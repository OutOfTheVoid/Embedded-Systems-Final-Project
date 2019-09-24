/*
Liam Taylor
wttesla@uw.edu

loop_delay.h:
Simple delay routine
====================
Contains loop_delay()
*/

#ifndef LOOP_DELAY_H
#define LOOP_DELAY_H

#include <stdint.h>

/*
loop_delay
---------------------------------------------------
Delay imprecisely for a given number of loop cycles

Parameters
==========
delay: the number of loop cycles to delay
*/
static inline void loop_delay(uint32_t delay) {
	for (volatile uint32_t i = 0; i < delay; i ++) {
	}
}

#endif /* LOOP_DELAY_H */
