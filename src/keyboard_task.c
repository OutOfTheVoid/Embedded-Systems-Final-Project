#include "keyboard_task.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "freertos_support.h"
#include "task.h"
#include "semphr.h"
#include "memutil.h"

#include "keyboard.h"

#define KB_TASK_STACK 100
#define KB_TASK_PRIORITY 20

keybaord_keys_t keyboard_data [2];
bool keyboard_read_buffer_is_zero;

void keyboard_task(void * unused);

bool keyboard_task_init() {
	memzero(keyboard_data, sizeof(keybaord_keys_t) * 2);
	keyboard_read_buffer_is_zero = true;
	
	if (xTaskCreate(& keyboard_task, "keyboard_task", KB_TASK_STACK, NULL, tskIDLE_PRIORITY + KB_TASK_PRIORITY, NULL) != pdPASS) {
		return false;
	}
	
	return true;
}

void keyboard_task(void * unused) {
	(void) unused;
	
	keyboard_init();
	memzero(keyboard_data, sizeof (keybaord_keys_t) * 2);
	
	keybaord_keys_t key_map;
	
	while (true) {
		keyboard_poll_keys(& key_map);
		
		uint32_t int_status = block_interrupts();
		keyboard_data[keyboard_read_buffer_is_zero ? 1 : 0] = key_map;
		restore_interrupts(int_status);
		
		taskYIELD();
	}
}

keybaord_keys_t * keyboard_get_next_keymap() {
	uint32_t int_status = block_interrupts();
	keyboard_read_buffer_is_zero = ! keyboard_read_buffer_is_zero;
	keybaord_keys_t * next_keymap = & keyboard_data[keyboard_read_buffer_is_zero ? 0 : 1];
	restore_interrupts(int_status);
	return next_keymap;
}