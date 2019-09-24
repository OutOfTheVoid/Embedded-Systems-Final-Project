#ifndef KEYBOARD_TASK_H
#define KEYBOARD_TASK_H

#include <stdbool.h>

#include "keyboard.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "freertos_support.h"
#include "task.h"
#include "semphr.h"

bool keyboard_task_init();

keybaord_keys_t * keyboard_get_next_keymap();

#endif
