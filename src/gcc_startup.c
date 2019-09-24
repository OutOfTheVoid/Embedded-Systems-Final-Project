#include <stdbool.h>
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "isr.h"

/*
Changing this changes the stack size
*/
#define STACK_SIZE 0x400

/*
Forward declaration of default and reset interrupt handlers.

_reset_isr: handles device startup and reset
_nmi_handler: handles non-maskable interrupts
_fault_handler: handles faults
*/
void _reset_isr();
static void _generic_interrupt_handler();
static void _nmi_handler();
static void _fault_handler();

/*
Forward declare main()
*/
extern void main();

/*
This is the stack
*/
unsigned char _stack_base[STACK_SIZE];

/*
This is the interrupt vector table itself
*/

extern void timer_interrupt_handler();

__attribute__ ((section(".isr_vector")))
void (* const startup_interrupt_handlers[155])() = {
	// 0: Initial stack pointer
	(void (*)()) ((uintptr_t) _stack_base + sizeof(_stack_base)),
	// 1: Reset
	_reset_isr,
	// 2: NMI
	_nmi_handler,
	// 3: Hard Fault
	_fault_handler,
	// 4: MMU Fault
	_generic_interrupt_handler,
	// 5: Bus Fault
	_generic_interrupt_handler,
	// 6: Usage Fault
	_generic_interrupt_handler,
	0,
	0,
	0,
	0,
	// 11: SVCall
	_generic_interrupt_handler,
	// 12: Debug Monitor
	_generic_interrupt_handler,
	0,
	// 14: PendSV
	_generic_interrupt_handler,
	// 15: Systick
	_generic_interrupt_handler,
};

__attribute__((section(".isr_vector_mutable"))) __attribute__((aligned(1024)))
void (* interrupt_handlers[155])();
/*
Linker information
*/
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

/*
reset isr: sets up the processor for execution
*/

void _reset_isr() {
	// copy initializer data
	uint32_t * segment_init_src = & _etext;
	uint32_t * segment_init_dst = & _data;
	while (segment_init_dst < & _edata) {
		* segment_init_dst = * segment_init_src;
		segment_init_dst ++;
		segment_init_src ++;
	}
	
	// zero the bss
	unsigned char * zero = (unsigned char *) & _bss;
	while (zero < (unsigned char *) & _ebss) {
		* zero = 0;
		zero ++;
	}
	
	// copy the isr vector
	for (int i = 0; i < 155; i ++) {
		interrupt_handlers[i] = (void (*)()) startup_interrupt_handlers[i];
	}
	disable_interrupts();
	// relocate the interrupt vector table
	write_hw_u32((void *)NVIC_BASE, NVIC_VTABLE, (uint32_t) interrupt_handlers);
	
	enable_interrupts();
	dsb();
	
	// enable the FPU
	uint32_t cpapr_old = read_hw_u32((void *) CPACR_BASE, CPAPR_OFF);
	write_hw_u32((void *) CPACR_BASE, CPAPR_OFF, cpapr_old | CPACR_CP10_FULL | CPACR_CP11_FULL);
	
	// call main
	main();
	
	// loop forever if main returns
	while(true);
}

/*
The default handlers all loop forever for debug purposes.
*/

static void _generic_interrupt_handler() {
	while (true);
}

static void _nmi_handler() {
	while (true);
}

static void _fault_handler() {
	while (true);
}
