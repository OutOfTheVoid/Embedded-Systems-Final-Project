/*
Liam Taylor
wttesla@uw.edu

clock.h:
TM4C123GH6PM clock driver
=========================
Contains routines for changing the system clock speed
*/

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"

/*
set_clock_80MHz
========================================================
Sets the main clock to be controlled by the PLL at 80MHz
*/
static inline void set_clock_80MHz() {
	// use RCC2 as the clock control register and bypass the PLL for now - we'll use it once it's running and stable
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_USE_RCC2 | SYSCTL_RCC2_PLL_BYPASS);
	// set the RCC to use the 16MHz oscillator for now
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC, (read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC) & ~ SYSCTL_RCC_SOURCE_MASK) | SYSCTL_RCC_SOURCE_16MHz);
	// set RCC2 to use the main oscillator as the oscillator source
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, (read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_OSCSRC2_MASK) | SYSCTL_RCC2_OSCSRC2_MAIN);
	// allow the PLL to run
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_PWRDN2);
	// configure the PLL run at 400MHz
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_DIV400);
	// configure the PLL divider to 2 * 2.5
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, (read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_SYSDIV2_MASK) | SYSCTL_RCC2_SYSDIV2_DIV400_2_5);
	// wait for the PLL to stabilize
	while((read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RIS) & SYSCTL_RIS_PLLLRIS) == 0);
	// stop bypassing the PLL
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_PLL_BYPASS);
	// record current clock speed
	extern uint32_t __clock_current_speed;
	__clock_current_speed = 80000000;
}


/*
set_clock_4MHz
==================================================
Sets the main clock to be controlled by PIOSC / 4.
16MHz / 4 = 4MHz
*/
static inline void set_clock_4MHz() {
	// use RCC2 as the clock control register
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_USE_RCC2);
	// bypass the PLL - we're not using it in 4MHz mode
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_PLL_BYPASS);
	// set RCC2 to use the main oscillator as the oscillator source
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, (read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_OSCSRC2_MASK) | SYSCTL_RCC2_OSCSRC2_PIOSCD4);
	// record current clock speed
	extern uint32_t __clock_current_speed;
	__clock_current_speed = 4000000;
}

/*
set_clock_16MHz
===================================================================
Sets the main clock to be controlled by the main oscillator (16MHz)
*/
static inline void set_clock_16MHz() {
	// use RCC2 as the clock control register
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_USE_RCC2);
	// bypass the PLL - we're not using it in 4MHz mode
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) | SYSCTL_RCC2_PLL_BYPASS);
	// set RCC2 to use the main oscillator as the oscillator source
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2, (read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC2) & ~ SYSCTL_RCC2_OSCSRC2_MASK) | SYSCTL_RCC2_OSCSRC2_MAIN);
	// record current clock speed
	extern uint32_t __clock_current_speed;
	__clock_current_speed = 16000000;
}

static inline uint32_t get_clock_speed_hz() {
	extern uint32_t __clock_current_speed;
	return __clock_current_speed;
}

#endif
