#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>

const uint32_t kBase_10 = 10;
const uint32_t kBase_2 = 2;
const uint32_t kBase_16 = 16;

static const char * const digit_str = "0123456789ABCDEF";

#define PRINT_CHAR_BUFF_SIZE_MIN 32

static inline uint32_t sprint_u32(uint32_t value, uint32_t base, char * buff) {
	if (base > 16)
		base = 16;
	if (base < 2)
		base = 2;
	
	uint32_t index = 0;
	do {
		buff[index] = digit_str[value % base];
		index ++;
		value /= base;
	} while (value != 0);
	
	for (int i = 0; i < index / 2; i ++) {
		char temp = buff[i];
		buff[i] = buff[index - i - 1];
		buff[index - i - 1] = temp;
	}
	return index;
}

#include "uart.h"

static inline void uart_print_u32(uint32_t value, uint32_t base, UART uart) {
	char buff[PRINT_CHAR_BUFF_SIZE_MIN];
	uint32_t print_size = sprint_u32(value, base, buff);
	uart_write(uart, buff, print_size);
}

static inline void uart_print_u32_padded(uint32_t value, uint32_t base, uint32_t pad_to, UART uart) {
	char buff[PRINT_CHAR_BUFF_SIZE_MIN];
	uint32_t print_size = sprint_u32(value, base, buff);
	uint32_t pad_size = 0;
	while (print_size + pad_size < pad_to) {
		uart_send_byte(uart, '0');
		pad_size ++;
	}
	uart_write(uart, buff, print_size);
}

#endif
