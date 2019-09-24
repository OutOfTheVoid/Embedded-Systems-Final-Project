#ifndef HALT_H
#define HALT_H

#include "isr.h"

static inline void halt() {
	disable_interrupts();
	while(1);
}

#endif
