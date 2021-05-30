################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
UART_DMA_IDLE_TEST.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
UART_DMA_IDLE_TEST.list \

OBJCOPY_BIN += \
UART_DMA_IDLE_TEST.bin \


# All Target
all: UART_DMA_IDLE_TEST.elf secondary-outputs

# Tool invocations
UART_DMA_IDLE_TEST.elf: $(OBJS) $(USER_OBJS) /media/d_win/Programowanie/workspace/UART_DMA_IDLE_TEST/STM32F411RETX_FLASH.ld
	arm-none-eabi-gcc -o "UART_DMA_IDLE_TEST.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/media/d_win/Programowanie/workspace/UART_DMA_IDLE_TEST/STM32F411RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="UART_DMA_IDLE_TEST.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

UART_DMA_IDLE_TEST.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "UART_DMA_IDLE_TEST.list"
	@echo 'Finished building: $@'
	@echo ' '

UART_DMA_IDLE_TEST.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "UART_DMA_IDLE_TEST.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
