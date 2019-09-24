#ifndef LOG_H
#define LOG_H

#include "uart.h"
#include "gpio.h"

/*
configuration
*/
#define LOG_UART kUART0_PA0_PA1
#define LOG_UART_DRIVE_CURRENT kDriveCurrent_2ma
#define LOG_UART_CLOCK_SOURCE kUARTClockSource_PIOSC_D_16
#define LOG_UART_PIN_DRIVE kPinDrive_PullDown
#define LOG_UART_BAUD_RATE 115200
#define LOG_UART_PARITY kUARTParity_None
#define LOG_UART_USES_TWO_STOP_BITS false

static inline void log_init() {
	uart_enable(LOG_UART);
	uart_setup(LOG_UART, LOG_UART_DRIVE_CURRENT, LOG_UART_PIN_DRIVE, LOG_UART_CLOCK_SOURCE, LOG_UART_BAUD_RATE, LOG_UART_PARITY, LOG_UART_USES_TWO_STOP_BITS, true, kUARTWordLength_8);
	uart_start(LOG_UART);
}

static inline uint32_t __log_strlen(const char * str) {
	uint32_t index = 0;
	while (str[index] != '\0') {
		index ++;
	}
	return index;
}

static inline void log_write_string(const char * message) {
	uart_write(LOG_UART, (char *) message, __log_strlen(message));
}

#endif
