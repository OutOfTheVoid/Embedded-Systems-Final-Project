#include "serial_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include "halt.h"

#include "audio_task.h"

#define SERIAL_STACK_DEPTH 100
#define SERIAL_PRIORITY 20

#define RCV_MESSAGE_PING 0
#define SND_MESSAGE_PING 0

#define RCV_MESSAGE_SET_OSC 0xFF

void serial_task();

TaskHandle_t serial_task_handle;

bool serial_task_init() {
	uart_enable(SERIAL_UART);
	uart_setup(SERIAL_UART, kDriveCurrent_2ma, kPinDrive_PullDown, kUARTClockSource_PIOSC_D_16, 115200, kUARTParity_None, false, true, kUARTWordLength_8);
	if (xTaskCreate(& serial_task, "serial_task", SERIAL_STACK_DEPTH, NULL, tskIDLE_PRIORITY + SERIAL_PRIORITY, & serial_task_handle) != pdTRUE) {
		return false;
	}
	return true;
}

void serial_task() {
	uint8_t receive_buff[32];
	int receive_pos = 0;
	bool started = false;
	uint8_t message_type = 0;
	uint8_t message_length = 0;
	uart_start(SERIAL_UART);
	while (true) {
		if (uart_rx_fifo_empty(SERIAL_UART)) {
			portYIELD();
		} else {
			if (! started) {
				started = true;
				uart_receive_byte(SERIAL_UART, & message_type);
				receive_pos = 0;
				switch (message_type)
				{
					case RCV_MESSAGE_PING: {
						started = false;
					} break;
					
					case RCV_MESSAGE_SET_OSC: {
						started = true;
						message_length = 1;
					} break;
					
					default: {
						started = false;
					} break;
				}
			} else {
				uart_receive_byte(SERIAL_UART, & receive_buff[receive_pos]);
				receive_pos ++;
				if (receive_pos == message_length) {
					switch (message_type) {
						case RCV_MESSAGE_SET_OSC: {
							uint32_t int_state = block_interrupts();
							voice_type = receive_buff[0];
							restore_interrupts(int_state);
							started = false;
						} break;
						
						default: {
							started = false;
							break;
						}
					}
				}
			}
		}
	}
}
