/*
Liam Taylor
wttesla@uw.edu

uart.h:
TM4C123GH6PM UART driver
========================
Contains routines for configuring and using the UART modules
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "gpio.h"
#include "clock.h"

typedef enum {
	kUART0_PA0_PA1 = 0,
	kUART1_PB0_PB1 = 1,
	kUART1_PC4_PC5 = 2,
	kUART2_PD6_PD7 = 3,
	kUART3_PC6_PC7 = 4,
	kUART4_PC4_PC5 = 5,
	kUART5_PE4_PE5 = 6,
	kUART6_PD4_PD5 = 7,
	kUART7_PE0_PE1 = 8,
} UART;

typedef struct {
	uint8_t module_num;
	void * base;
	Port port;
	uint8_t rx_pin;
	uint8_t tx_pin;
	uint8_t function;
} UARTInfo;

typedef enum {
	kUARTClockSource_MainClock_D_16 = 0,
	kUARTClockSource_MainClock_D_8 = UART_CTL_CLOCKDIV_8,
	kUARTClockSource_PIOSC_D_16 = UART_CC_PIOSC,
	kUARTClockSOurce_PIOSC_D_8 = UART_CC_PIOSC | UART_CTL_CLOCKDIV_8
} UARTClockSource;

typedef enum {
	kUARTParity_None,
	kUARTParity_Even,
	kUARTParity_Odd
} UARTParity;

typedef enum {
	kUARTWordLength_5 = UART_LCRH_WORD_LEN_5,
	kUARTWordLength_6 = UART_LCRH_WORD_LEN_6,
	kUARTWordLength_7 = UART_LCRH_WORD_LEN_7,
	kUARTWordLength_8 = UART_LCRH_WORD_LEN_8,
} UARTWordLength;

#define kUARTRXError_Framing 0x100
#define kUARTRXError_Parity 0x200
#define kUARTRXError_Break 0x400
#define kUARTRXError_Overrun 0x800
#define kUARTRXError_Success 0x000

static UARTInfo __uart_info[] = {
	[kUART0_PA0_PA1] = {.module_num = 0, .base = (void *) UART_UART0_BASE, .port = kPortA, .rx_pin = 0, .tx_pin = 1, .function = 1},
	[kUART1_PB0_PB1] = {.module_num = 1, .base = (void *) UART_UART1_BASE, .port = kPortB, .rx_pin = 0, .tx_pin = 1, .function = 1},
	[kUART1_PC4_PC5] = {.module_num = 1, .base = (void *) UART_UART1_BASE, .port = kPortC, .rx_pin = 4, .tx_pin = 5, .function = 2},
	[kUART2_PD6_PD7] = {.module_num = 2, .base = (void *) UART_UART2_BASE, .port = kPortD, .rx_pin = 6, .tx_pin = 7, .function = 1},
	[kUART3_PC6_PC7] = {.module_num = 3, .base = (void *) UART_UART3_BASE, .port = kPortC, .rx_pin = 6, .tx_pin = 7, .function = 1},
	[kUART4_PC4_PC5] = {.module_num = 4, .base = (void *) UART_UART4_BASE, .port = kPortC, .rx_pin = 4, .tx_pin = 5, .function = 1},
	[kUART5_PE4_PE5] = {.module_num = 5, .base = (void *) UART_UART5_BASE, .port = kPortE, .rx_pin = 4, .tx_pin = 5, .function = 1},
	[kUART6_PD4_PD5] = {.module_num = 6, .base = (void *) UART_UART6_BASE, .port = kPortD, .rx_pin = 4, .tx_pin = 5, .function = 1},
	[kUART7_PE0_PE1] = {.module_num = 7, .base = (void *) UART_UART7_BASE, .port = kPortE, .rx_pin = 0, .tx_pin = 1, .function = 1},
};

static inline void uart_enable(UART uart) {
	uint32_t rcgc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCUART);
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCUART, rcgc_old | (1 << __uart_info[uart].module_num));
	gpio_enable_port(__uart_info[uart].port);
}

static inline void uart_stop(UART uart) {
	uint32_t ctl_old = read_hw_u32(__uart_info[uart].base, UART_CTL);
	write_hw_u32(__uart_info[uart].base, UART_CTL, ctl_old & ~ UART_CTL_ENABLE);
}

static inline void uart_start(UART uart) {
	uint32_t ctl_old = read_hw_u32(__uart_info[uart].base, UART_CTL);
	write_hw_u32(__uart_info[uart].base, UART_CTL, ctl_old | UART_CTL_ENABLE);
}

static inline void uart_setup(UART uart, DriveCurrent drive_current, PinDrive pin_drive, UARTClockSource clock_source, uint32_t baud_rate, UARTParity parity, bool two_stop_bits, bool enable_fifo, UARTWordLength word_length) {
	gpio_setup_digital_pin_group_alternate_function(__uart_info[uart].port, 1 << __uart_info[uart].rx_pin, __uart_info[uart].function, false, drive_current, pin_drive);
	gpio_setup_digital_pin_group_alternate_function(__uart_info[uart].port, 1 << __uart_info[uart].tx_pin, __uart_info[uart].function, true, drive_current, pin_drive);
	gpio_enable_digital_pin_group(__uart_info[uart].port, (1 << __uart_info[uart].rx_pin) | (1 << __uart_info[uart].tx_pin));
	
	uint32_t clock_speed = (clock_source & UART_CC_SYSTEM_CLOCK) ? get_clock_speed_hz() : 16000000;
	clock_speed /= (clock_source & UART_CTL_CLOCKDIV_8) ? 8 : 16;
	uint32_t ibrd = clock_speed / baud_rate;
	uint32_t fbrd = ((clock_speed << 6) / baud_rate) & 0x3F;
	uint32_t ibrd_old = read_hw_u32(__uart_info[uart].base, UART_IBRD);
	uint32_t fbrd_old = read_hw_u32(__uart_info[uart].base, UART_FBRD);
	write_hw_u32(__uart_info[uart].base, UART_IBRD, (ibrd_old & ~ UART_IBRD) | ibrd);
	write_hw_u32(__uart_info[uart].base, UART_FBRD, (fbrd_old & ~ UART_FBRD) | fbrd);
	
	uint32_t lcrh_old = read_hw_u32(__uart_info[uart].base, UART_LCRH);
	lcrh_old &= ~ (UART_LCRH_WORD_LEN_MASK | UART_LCRH_FIFO_EN | UART_LCRH_TWO_STOP | UART_LCRH_PARITY_EVEN | UART_LCRH_PARITY_EN | UART_LCRH_SEND_BREAK);
	if (parity != kUARTParity_None) {
		lcrh_old |= UART_LCRH_PARITY_EN;
		if (parity == kUARTParity_Even)
			lcrh_old |= UART_LCRH_PARITY_EVEN;
	}
	if (two_stop_bits)
		lcrh_old |= UART_LCRH_TWO_STOP;
	if (enable_fifo)
		lcrh_old |= UART_LCRH_FIFO_EN;
	lcrh_old |= word_length;
	write_hw_u32(__uart_info[uart].base, UART_LCRH, lcrh_old);
	
	uint32_t cc_old = read_hw_u32(__uart_info[uart].base, UART_CC);
	write_hw_u32(__uart_info[uart].base, UART_CC, (cc_old & ~ UART_CC_MASK) | (clock_source & UART_CC_MASK));
}

static inline uint32_t uart_receive_byte(UART uart, uint8_t * data) {
	uint32_t data_and_status = read_hw_u32(__uart_info[uart].base, UART_DR);
	* data = data_and_status & 0xFF;
	return data_and_status & 0xF00;
}

static inline void uart_send_byte(UART uart, uint8_t byte) {
	uint32_t dr_old = read_hw_u32(__uart_info[uart].base, UART_DR);
	write_hw_u32(__uart_info[uart].base, UART_DR, (dr_old & ~ 0xFF) | ((uint32_t) byte));
}

static inline void uart_clear_error(UART uart) {
	uint32_t rcr_ecr_old = read_hw_u32(__uart_info[uart].base, UART_RSR_ECR);
	write_hw_u32(__uart_info[uart].base, UART_RSR_ECR, rcr_ecr_old | 0xFF);
}

static inline bool uart_rx_fifo_empty(UART uart) {
	return read_hw_u32(__uart_info[uart].base, UART_FR) & UART_FR_RXFIFO_EMPTY;
}

static inline bool uart_rx_fifo_full(UART uart) {
	return read_hw_u32(__uart_info[uart].base, UART_FR) & UART_FR_RXFIFO_FULL;
}

static inline bool uart_tx_fifo_empty(UART uart) {
	return read_hw_u32(__uart_info[uart].base, UART_FR) & UART_FR_TXFIFO_EMPTY;
}

static inline bool uart_tx_fifo_full(UART uart) {
	return read_hw_u32(__uart_info[uart].base, UART_FR) & UART_FR_TXFIFO_FULL;
}

static inline void uart_write(UART uart, char * data, uint32_t length) {
	uint32_t i = 0;
	while (i < length) {
		while (uart_tx_fifo_full(uart));
		uart_send_byte(uart, (uint8_t) data[i]);
		i ++;
	}
}

#endif
