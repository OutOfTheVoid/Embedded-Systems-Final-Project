#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#include <stdbool.h>
#include "uart.h"

#define SERIAL_UART kUART0_PA0_PA1

bool serial_task_init();

#endif

