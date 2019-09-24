#ifndef FREERTOS_SUPPORT_H
#define FREERTOS_SUPPORT_H

#include "isr.h"
#include "tm4c123gh6pm.h"

extern void xPortPendSVHandler();
extern void vPortSVCHandler();
extern void xPortSysTickHandler();

static inline void freertos_setup() {
	uint32_t int_state = block_interrupts();
	interrupt_handlers[INTR_SVCALL] = & vPortSVCHandler;
	interrupt_handlers[INTR_SYSTICK] = & xPortSysTickHandler;
	interrupt_handlers[INTR_PENDSV] = & xPortPendSVHandler;
	restore_interrupts(int_state);
}

#endif
