/*
 * uart.c
 *
 *  Created on: 25.10.2019
 *      Author: Marcin
 */

#include "main.h"
#include "uart_dma.h"
#include <string.h>
#include <stdio.h>
#define RX_CHECK 0
#define TX_CHECK 1

//variables
UART_BufferTypeDef *uart_tab[3] = { NULL, NULL, NULL };
uint8_t uart_tab_idx = 0;
extern UART_BufferTypeDef uart_pc;

void Serial_init(UART_BufferTypeDef *uart, UART_HandleTypeDef *huart) {
	//read_tail = 0;
	uart->huart = huart;
	uart->rx_tail = 0;
	uart->rx_head = 0;
	uart->tx_tail = 0;
	uart->tx_head = 0;
	uart->tx_message_idx_head = 0;
	uart->tx_message_idx_tail = 0;
	uart->tx_message_size = 0;
	uart->tx_transmit = 0;
	uart_tab[uart_tab_idx] = uart;
	uart_tab_idx++;
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	__HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
	Serial_receive(uart, RX_BUFFER_SIZE);
}

void Serial_receive(UART_BufferTypeDef *uart, uint16_t msg_size) {
	HAL_UART_Receive_DMA(uart->huart, uart->rx_buffer, msg_size);
}

void Write_RX_buff(UART_BufferTypeDef *uart, uint8_t size) {
	uint8_t temp_head;
	for (uint8_t i = 0; i < size; i++) {
		temp_head = (uart->rx_head + 1) % RX_BUFFER_SIZE;
		if (temp_head == uart->rx_tail) {
			break;
		} else {
			uart->rx_work_buff[uart->rx_head] = uart->rx_buffer[i];
			uart->rx_head = temp_head;
		}
	}
}

uint8_t Serial_read_char(UART_BufferTypeDef *uart) {
	uint8_t r = uart->rx_work_buff[uart->rx_tail];
	uart->rx_tail = (uart->rx_tail + 1) % RX_BUFFER_SIZE;
	return r;
}

void Serial_read_line(UART_BufferTypeDef *uart, char* buffer) {
	uint8_t c;
	while(Serial_available(uart)) {
		c = Serial_read_char(uart);
		*buffer = (char)c;
		buffer++;
		if((char)c == '\n') {
			*buffer = '\0';
			break;
		}
	}
}

void Serial_read_chars(UART_BufferTypeDef *uart, char* buffer, uint16_t quantity) {
	uint16_t j;
	uint16_t avail_char = Serial_available(uart);
	if (quantity > avail_char) {
		j = avail_char;
	}
	else {
		j = quantity;
	}
	for(uint16_t i = 0; i < j; i++) {
		*buffer = (char)Serial_read_char(uart);
		buffer++;
	}
}


uint8_t Serial_available(UART_BufferTypeDef *uart) {
	uint8_t bytes;
	if (uart->rx_head >= uart->rx_tail) {
		bytes = uart->rx_head - uart->rx_tail;
	} else {
		bytes = RX_BUFFER_SIZE - uart->rx_tail + uart->rx_head;
	}
	return bytes;
}

void Serial_RxCplt(UART_BufferTypeDef *uart) {
	uint8_t size = RX_BUFFER_SIZE - uart->huart->hdmarx->Instance->NDTR;
	Write_RX_buff(uart, size);
	Serial_receive(uart, RX_BUFFER_SIZE);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	Serial_checkCallback(huart, RX_CHECK);
}
void Serial_print(UART_BufferTypeDef *uart, char* message) {
	uint16_t size = strlen(message);
	Serial_send(uart, (uint8_t *)message, size);
}

void Serial_print_int(UART_BufferTypeDef *uart, int integer) {
	char message[12];
	int size;
	size = sprintf(message, "%d", integer);
	Serial_send(uart, (uint8_t *)message, (uint16_t) size);
}

void Serial_print_float(UART_BufferTypeDef *uart, float fn) {
	char message[12];
	int size;
	size = sprintf(message, "%f", fn);
	Serial_send(uart, (uint8_t *)message, (uint16_t) size);
}

void Serial_println(UART_BufferTypeDef *uart) {
	Serial_send(uart, (uint8_t *)"\n\r", 2);
}

void Serial_send(UART_BufferTypeDef *uart, uint8_t *message, uint16_t size) {
	uint8_t tx_buffer_free;
	if(size > TX_BUFFER_SIZE) {
		size = TX_BUFFER_SIZE;
	}
	tx_buffer_free = TX_BUFFER_SIZE - uart->tx_head;
	if (size < tx_buffer_free) {
		uint16_t head = uart->tx_head;
		memcpy(uart->tx_buffer + uart->tx_head, message, size);
		if (!(uart->tx_transmit)) {
			HAL_UART_Transmit_DMA(uart->huart,
					(uart->tx_buffer + uart->tx_head), size);
			uart->tx_head = head + size;
			uart->tx_message_size += size;
			(uart->tx_transmit)++;
		} else {
			uart->tx_head = head + size;
		}
	} else {
		while (uart->tx_tail != uart->tx_head) {
			;
		}
		memcpy(uart->tx_buffer, message, size);
		HAL_UART_Transmit_DMA(uart->huart, uart->tx_buffer, size);
		uart->tx_tail = 0;
		uart->tx_head += size;
		uart->tx_message_size += size;
		(uart->tx_transmit)++;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	Serial_checkCallback(huart, TX_CHECK);
}

void Serial_TxCplt(UART_BufferTypeDef *uart) {
	uart->tx_tail += uart->tx_message_size;
	if (uart->tx_tail != uart->tx_head) {
		HAL_UART_Transmit_DMA(uart->huart, (uart->tx_buffer + uart->tx_tail),
				(uart->tx_head - uart->tx_tail));
		uart->tx_message_size = uart->tx_head - uart->tx_tail;
		uart->tx_transmit--;
	} else {
		uart->tx_message_size = 0;
		uart->tx_transmit = 0;
		uart->tx_tail = 0;
		uart->tx_head = 0;
	}
}

void User_UART_IRQHandler(UART_HandleTypeDef *huart) {
	volatile uint32_t tmp;
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		tmp = huart->Instance->SR;
		tmp = huart->Instance->DR;
		huart->hdmarx->Instance->CR &= ~DMA_SxCR_EN;
		tmp = tmp;
	}
}

void Serial_checkCallback(UART_HandleTypeDef *huart, uint8_t mode) {
	for (uint8_t i = 0; i < UART_PORTS; i++) {
		if (uart_tab[i] == NULL) {
			break;
		}
		else {
			if (huart == uart_tab[i]->huart) {
				if (mode == RX_CHECK) {
					Serial_RxCplt(uart_tab[i]);
				} else {
					Serial_TxCplt(uart_tab[i]);
				}
			}
		}
	}
}

