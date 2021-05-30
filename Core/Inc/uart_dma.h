/*
 * uart.h
 *
 *  Created on: 25.10.2019
 *      Author: Marcin
 */
#ifndef UART_DMA_H_
#define UART_DMA_H_

#include "stm32f4xx_hal.h"
#include "main.h"



//RX Buffer default size
#ifndef READ_BUFFER
#define READ_BUFFER 128
#endif

#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 256
#endif
//TX Buffer default size
#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 256
#endif

#ifndef MAX_TX_MESSAGES
#define MAX_TX_MESSAGES 10
#endif

//Number of UART ports
#ifndef UART_PORTS
#define UART_PORTS 3
#endif

//variables
typedef struct __UART_BufferTypeDef
{
	UART_HandleTypeDef *huart;
	uint8_t rx_buffer[RX_BUFFER_SIZE];
	uint8_t rx_work_buff[RX_BUFFER_SIZE];
	uint8_t rx_tail;
	uint8_t rx_head;
	uint8_t new_message;
	uint8_t tx_buffer[TX_BUFFER_SIZE];
	uint8_t tx_tail;
	uint8_t tx_head;
	uint8_t tx_message_idx_head;
	uint8_t tx_message_idx_tail;
	uint8_t tx_message_tails[MAX_TX_MESSAGES];
	uint8_t tx_message_size;
	uint8_t tx_transmit;
} UART_BufferTypeDef;

UART_BufferTypeDef *uart_tab[UART_PORTS];
uint8_t uart_tab_idx;


//functions
void Serial_init(UART_BufferTypeDef *uart, UART_HandleTypeDef *huart);
uint8_t Serial_read_char(UART_BufferTypeDef *uart);
void Serial_read_line(UART_BufferTypeDef *uart, char* buffer);
void Serial_read_chars(UART_BufferTypeDef *uart, char* buffer, uint16_t quantity);
uint8_t Serial_available(UART_BufferTypeDef *uart);
void Serial_print(UART_BufferTypeDef *uart, char* message);
void Serial_print_int(UART_BufferTypeDef *uart, int integer);
void Serial_println(UART_BufferTypeDef *uart);
void Serial_send(UART_BufferTypeDef *uart, uint8_t *message, uint16_t size);
void Serial_receive(UART_BufferTypeDef *uart, uint16_t msg_size);
void Serial_TxCplt(UART_BufferTypeDef *uart);
void Serial_RxCplt(UART_BufferTypeDef *uart);
void Serial_checkCallback(UART_HandleTypeDef *huart, uint8_t mode);
void User_UART_IRQHandler(UART_HandleTypeDef *huart);

#endif /* UART_H_ */
