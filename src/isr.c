#include "isr.h"

__attribute__((naked)) uint32_t block_interrupts() {
	uint32_t rval = 0;
	
	__asm__ volatile("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n"
          : "=r" (rval));
	
	return rval;
}

void restore_interrupts(uint32_t int_state) {
	if (!(int_state & 1)) {
		__asm__ volatile ("cpsie i\n");
	}
}
