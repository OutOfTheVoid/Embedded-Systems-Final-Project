/*
Liam Taylor
wttesla@uw.edu

isr.h:
TM4C123GH6PM interrupt driver
=============================
Contains routines for controlling interrupts and interrupt handlers
*/

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"

/*
disable_interrupts
---------------------------------------------------------------------------
Disables interrupts - specifically, disables interrupts and maskable faults
*/
static inline void disable_interrupts() {
	__asm__ volatile ("cpsid if");
}

/*
disable_interrupts
-------------------------------------------------------------------------
Enables interrupts - specifically, enables interrupts and maskable faults
*/
static inline void enable_interrupts() {
	__asm__ volatile ("cpsie if");
}

/*
dsb
-----------------------------------------------------
data barrier instruction - used to flush cpu pipeline
*/
static inline void dsb() {
	__asm__ volatile ("dsb");
}

/*
block_interrupts
------------------------------------------------------------
disables interrupts and returns the previous interrupt state

Return
======
Whether or not the interrupt status was set
*/
__attribute__((naked)) uint32_t block_interrupts();

/*
restore_interrupts
-----------------------------------------------------------------------
Restores interrupts to the state they were in before block_interrupts()

Parameters
==========
int_state: must be the value returned by the last call to block_interrupts()
*/
void restore_interrupts(uint32_t int_state);

extern void (*interrupt_handlers[])();

/*
set_isr
------------------------------------------------
Sets the interrupt handler for a given isr index

Parameters
==========
index: which isr to set
isr: the handler function address to set
*/
static inline void set_isr(uint32_t index, void (*isr)()) {
	index += 16;
	if (index >= 155)
		return;
	//int32_t int_state = block_interrupts();
	interrupt_handlers[index] = isr;
	//restore_interrupts(int_state);
	dsb();
}

/*
set_isr_enabled
----------------------------------------------
Enables an interrupt to be delivered to an ISR

Parameter
=========
index: the number of the isr to configure
*/
static inline void set_isr_enabled(uint32_t index) {
	if (index < 32) {
		uint32_t bit = 1 << index;
		write_hw_u32((void *) NVIC_BASE, NVIC_EN0, bit);
	} else if (index < 64) {
		uint32_t bit = 1 << (index - 32);
		write_hw_u32((void *) NVIC_BASE, NVIC_EN1, bit);
	} else if (index <= 95) {
		uint32_t bit = 1 << (index - 64);
		write_hw_u32((void *) NVIC_BASE, NVIC_EN2, bit);
	} else if (index <= 128) {
		uint32_t bit = 1 << (index - 96);
		write_hw_u32((void *) NVIC_BASE, NVIC_EN3, bit);
	} else if (index <= 138) {
		uint32_t bit = 1 << (index - 128);
		write_hw_u32((void *) NVIC_BASE, NVIC_EN4, bit);
	}
}

/*
set_isr_disabled
-----------------------------------------------
Disables an interrupt being delivered to an ISR

Parameter
=========
index: the number of the isr to configure
*/
static inline void set_isr_disabled(uint32_t index) {
	if (index < 32) {
		uint32_t bit = 1 << index;
		write_hw_u32((void *) NVIC_BASE, NVIC_DIS0, bit);
	} else if (index < 64) {
		uint32_t bit = 1 << (index - 32);
		write_hw_u32((void *) NVIC_BASE, NVIC_DIS1, bit);
	} else if (index <= 95) {
		uint32_t bit = 1 << (index - 64);
		write_hw_u32((void *) NVIC_BASE, NVIC_DIS2, bit);
	} else if (index <= 128) {
		uint32_t bit = 1 << (index - 96);
		write_hw_u32((void *) NVIC_BASE, NVIC_DIS3, bit);
	} else if (index <= 138) {
		uint32_t bit = 1 << (index - 128);
		write_hw_u32((void *) NVIC_BASE, NVIC_DIS4, bit);
	}
}

#endif /* ISR_H */
