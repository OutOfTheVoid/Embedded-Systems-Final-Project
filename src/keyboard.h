#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#include "gpio.h"
#include "loop_delay.h"

#define KEY_C 1
#define KEY_Cs 2
#define KEY_D 4
#define KEY_Ds 8
#define KEY_E 16
#define KEY_F 32
#define KEY_Fs 64
#define KEY_G 128
#define KEY_Gs 256
#define KEY_A 512
#define KEY_As 1024
#define KEY_B 2048

/*
Keyboard mux mappings:

Data read pins:   PC4,  PC5,  PC6,  PC7,  PD6,  PD7
                  C/Fs  Cs/G  D/Gs  Ds/A  E/As  F/B

Chip select pins: PE0, PE1, PE2, PE3
                  Oct 1   | Oct 2   
*/

#define KEYBOARD_READ_DELAY 2000

typedef struct {
	Port port;
	uint8_t pin;
} PinMapping;

static PinMapping _keyboard_select_map[] = {
	{.port = kPortE, .pin = 1},
	{.port = kPortE, .pin = 2},
};

static PinMapping _keyboard_data_map[] = {
	{.port = kPortC, .pin = 4},
	{.port = kPortC, .pin = 5},
	{.port = kPortC, .pin = 6},
	{.port = kPortC, .pin = 7},
	{.port = kPortD, .pin = 6},
	{.port = kPortD, .pin = 7},
};

typedef struct {
	uint16_t key_map;
} keybaord_keys_t;

static inline void keyboard_init() {
	for (int i = 0; i < 2; i ++) {
		gpio_enable_port(_keyboard_select_map[i].port);
		gpio_setup_digital_output_pin(_keyboard_select_map[i].port, _keyboard_select_map[i].pin, kDriveCurrent_8ma, kPinDrive_PullDown);
		gpio_enable_digital_pin(_keyboard_select_map[i].port, _keyboard_select_map[i].pin);
		gpio_set_digital_output_pin(_keyboard_select_map[i].port, _keyboard_select_map[i].pin, false);
	}
	for (int i = 0; i < 6; i ++) {
		gpio_enable_port(_keyboard_data_map[i].port);
		gpio_setup_digital_input_pin(_keyboard_data_map[i].port, _keyboard_data_map[i].pin, kPinDrive_OpenDrain);
		gpio_enable_digital_pin(_keyboard_data_map[i].port, _keyboard_data_map[i].pin);
	}
}

static inline void keyboard_poll_keys(keybaord_keys_t * keys) {
	gpio_set_digital_output_pin(_keyboard_select_map[0].port, _keyboard_select_map[0].pin, true);
	loop_delay(KEYBOARD_READ_DELAY);
	keys -> key_map = 0;
	for (int k = 0; k < 6; k ++) {
		keys -> key_map |= (! gpio_get_digital_pin(_keyboard_data_map[k].port, _keyboard_data_map[k].pin)) ? (1 << k) : 0;
	}
	gpio_set_digital_output_pin(_keyboard_select_map[0].port, _keyboard_select_map[0].pin, false);
	gpio_set_digital_output_pin(_keyboard_select_map[1].port, _keyboard_select_map[1].pin, true);
	loop_delay(KEYBOARD_READ_DELAY);
	for (int k = 0; k < 6; k ++) {
		keys -> key_map |= (! gpio_get_digital_pin(_keyboard_data_map[k].port, _keyboard_data_map[k].pin)) ? (1 << (k + 6)) : 0;
	}
	gpio_set_digital_output_pin(_keyboard_select_map[1].port, _keyboard_select_map[1].pin, false);
	loop_delay(KEYBOARD_READ_DELAY);
}

#endif
