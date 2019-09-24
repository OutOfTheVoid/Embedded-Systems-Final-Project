#ifndef TIMER_DELAY_H
#define TIMER_DELAY_H

#include "timer.h"

/*
timer_delay:
-----------------------------------------
using a timer, wait for a period of time.

Parameters
----------
timer: the timer to use for timing
cycles: the number of scaled cycles of the timer to wait
prescale: the cycle divisor (1 + prescale)

Preconditions
-------------
- Timer timer must be enabled with timer_enable() (timer.h)
  before calling this function
*/

static inline void timer_delay(Timer timer, uint32_t cycles) {
	timer_setup_oneshot_solo(timer);
	timer_set_interval_solo(timer, cycles);
	timer_reset_timeout_solo(timer);
	timer_disable_interrupt_solo(timer);
	timer_run_solo(timer);
	while (!timer_poll_timeout_solo(timer));
	timer_reset_timeout_solo(timer);
}

#endif
