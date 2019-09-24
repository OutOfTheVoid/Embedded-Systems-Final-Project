/*
Liam Taylor
wttesla@uw.edu

gpio.h:
TM4C123GH6PM gpio driver
========================
Contains routines for initializing and controlling GPIO pins
*/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "isr.h"

// uncomment this line to use the Advanced High-Performance Bus 
//#define GPIO_USE_AHB 1

typedef enum {
#ifdef GPIO_USE_AHB
	kPortA = GPIO_PORTA_AHB_BASE,
	kPortB = GPIO_PORTB_AHB_BASE,
	kPortC = GPIO_PORTC_AHB_BASE,
	kPortD = GPIO_PORTD_AHB_BASE,
	kPortE = GPIO_PORTE_AHB_BASE,
	kPortF = GPIO_PORTF_AHB_BASE,
#else
	kPortA = GPIO_PORTA_BASE,
	kPortB = GPIO_PORTB_BASE,
	kPortC = GPIO_PORTC_BASE,
	kPortD = GPIO_PORTD_BASE,
	kPortE = GPIO_PORTE_BASE,
	kPortF = GPIO_PORTF_BASE,
#endif
} Port;

#define PINMASK_0 0x01
#define PINMASK_1 0x02
#define PINMASK_2 0x04
#define PINMASK_3 0x08
#define PINMASK_4 0x10
#define PINMASK_5 0x20
#define PINMASK_6 0x40
#define PINMASK_7 0x80
#define PINMASK_ALL 0xFF

typedef enum {
	kDriveCurrent_2ma,
	kDriveCurrent_4ma,
	kDriveCurrent_8ma,
	kDriveCurrent_12ma
} DriveCurrent;

typedef enum {
	kPinDrive_PullUp,
	kPinDrive_PullDown,
	kPinDrive_OpenDrain
} PinDrive;

typedef enum {
	kPinInterruptType_RisingEdge,
	kPinInterruptType_FallingEdge,
	kPinInterruptType_BothEdges,
	kPinInterruptType_HighLevel,
	kPinInterruptType_LowLevel
} PinInterruptType;

/*
Maps ports to bits in RCGCGPIO
(page 340 in TM4C123GH6PM manual)
*/
static inline uint32_t __gpio_port_to_RCGCGPIO_bit(Port port) {
	switch(port) {
		case kPortA:
			return 0x01;
		case kPortB:
			return 0x02;
		case kPortC:
			return 0x04;
		case kPortD:
			return 0x08;
		case kPortE:
			return 0x10;
		case kPortF:
			return 0x20;
		default:
			return 0;
	}
}

static inline uint32_t __gpio_port_to_interrupt_handler(Port port) {
	switch(port) {
		case kPortA:
			return INTR_GPIO_PORTA;
		case kPortB:
			return INTR_GPIO_PORTB;
		case kPortC:
			return INTR_GPIO_PORTC;
		case kPortD:
			return INTR_GPIO_PORTD;
		case kPortE:
			return INTR_GPIO_PORTE;
		case kPortF:
			return INTR_GPIO_PORTF;
		default:
			return 156;
	}
}

/*
gpio_enable_port:
-------------------
Enables a gpio port

Parameters
==========
port: the port to enable
*/
static inline void gpio_enable_port (Port port) {
	uint32_t bit = __gpio_port_to_RCGCGPIO_bit(port);
	uint32_t old = read_hw_u32((void *) SYSCTL_BASE, 0x608);
	write_hw_u32((void *) SYSCTL_BASE, 0x608, old | bit);
}

/*
gpio_is_port_enabled:
---------------------------------
Checks if a given port is enabled

Parameters
==========
port: the port to check

Returns
=======
a boolean indicating if the port is enabled
*/
static inline bool gpio_is_port_enabled (Port port) {
	uint32_t bit = __gpio_port_to_RCGCGPIO_bit(port);
	uint32_t old = read_hw_u32((void *) SYSCTL_BASE, 0x608);
	return bit & old;
}

/*
gpio_setup_digital_output_pin_group:
----------------------------------------------
Configures a set of pins to be digital outputs

Parameters
==========
port: port of pin to configure
pin_mask: mask representing pins to configure
current: the drive current for these pins
drive: the drive mode for these pins

Preconditions
=============
- Requires that the port is enabled (gpio_enable_port)
*/
static inline void gpio_setup_digital_output_pin_group(Port port, uint8_t pin_mask, DriveCurrent current, PinDrive drive) {
	// record old direction and afsel/amsel register values
	uint32_t old_dir = read_hw_u32((void *)port, GPIO_DIR);
	uint32_t old_afsel = read_hw_u32((void *)port, GPIO_AFSEL);
	uint32_t old_amsel = read_hw_u32((void *) port, GPIO_AMSEL);
	// set output direction for these pins
	write_hw_u32((void *)port, GPIO_DIR, old_dir | pin_mask);
	// disable alternate-function mode for this pin
	write_hw_u32((void *)port, GPIO_AFSEL, old_afsel & ~pin_mask);
	// enable the analog isolation for these pins
	write_hw_u32((void *) port, GPIO_AMSEL, old_amsel & ~pin_mask);
	
	// read the old current register of a given type,
	// then write it back with this pin enabled for the current
	// selected by <current>
	uint32_t old_current = 0;
	switch (current) {
		case kDriveCurrent_12ma:
			old_current = read_hw_u32((void *)port, GPIO_DR12R);
			write_hw_u32((void *)port, GPIO_DR12R, old_current | pin_mask);
			break;
		
		case kDriveCurrent_8ma:
			old_current = read_hw_u32((void *)port, GPIO_DR8R);
			write_hw_u32((void *)port, GPIO_DR8R, old_current | pin_mask);
			break;
			
		case kDriveCurrent_4ma:
			old_current = read_hw_u32((void *)port, GPIO_DR4R);
			write_hw_u32((void *)port, GPIO_DR4R, old_current | pin_mask);
			break;
			
		case kDriveCurrent_2ma:
		default:
			old_current = read_hw_u32((void *)port, GPIO_DR2R);
			write_hw_u32((void *)port, GPIO_DR2R, old_current | pin_mask);
			break;
	}
	
	// It's faster to just unlock the pin always than check for special pins
	// so unlock the gpio lock in case this pin is a locked pin
	uint32_t old_lock = read_hw_u32((void *)port, GPIO_LOCK);
	write_hw_u32((void *)port, GPIO_LOCK, GPIO_LOCK_KEYVAL);
	// and permit write to the pin via the GPIO_CR register
	uint32_t old_cr = read_hw_u32((void *)port, GPIO_CR);
	write_hw_u32((void *)port, GPIO_CR, old_cr | pin_mask);
	
	// store the old drive register(s) value(s) then write it back with these
	// pins enabled (or disabled) as required
	uint32_t old_drive = 0;
	switch (drive) {
		case kPinDrive_PullUp:
			old_drive = read_hw_u32((void *)port, GPIO_PUR);
			write_hw_u32((void *)port, GPIO_PUR, old_drive | pin_mask);
			break;
			
		case kPinDrive_PullDown:
			old_drive = read_hw_u32((void *)port, GPIO_PDR);
			write_hw_u32((void *)port, GPIO_PDR, old_drive | pin_mask);
			break;
		
		case kPinDrive_OpenDrain:
		default:
			old_drive = read_hw_u32((void *)port, GPIO_PUR);
			write_hw_u32((void *)port, GPIO_PUR, old_drive & ~pin_mask);
			old_drive = read_hw_u32((void *)port, GPIO_PDR);
			write_hw_u32((void *)port, GPIO_PDR, old_drive & ~pin_mask);
			break;
	}
	
	// restore the CR state now that we're done modifying the pins' config
	write_hw_u32((void *)port, GPIO_CR, old_cr);
	// restore the lock state now that we're done with CR
	if (old_lock == 1) {
		write_hw_u32((void *)port, GPIO_LOCK, 0);
	}
}

/*
gpio_setup_digital_output_pin:
--------------------------------------------
Configures a pin to be a digital output pin

Parameters
==========
port: port of pin to configure
pin: pin number in port to configure
current: pin drive current
drive: pin drive mode

Preconditions
=============
- Requires that the port is enabled (gpio_enable_port)
*/
static inline void gpio_setup_digital_output_pin(Port port, uint8_t pin, DriveCurrent current, PinDrive drive) {
	// the pin mask for pin n is just 1 << n, so use the group function rather than duplicating a ton of code.
	gpio_setup_digital_output_pin_group(port, 1 << pin, current, drive);
}

/*
gpio_enable_digital_pin_group:
------------------------------
enables a group of pins

Parameters:
===========
port: the port the pins belong to
pin_mask: mask representing pins to enable

Preconditions
=============
- Requires that the pins enabled are set up as digital pins (in or out)
*/
static inline void gpio_enable_digital_pin_group(Port port, uint8_t pin_mask) {
	// record the old enable register value
	uint32_t old_enable = read_hw_u32((void *)port, GPIO_DEN);
	// set the bits for these pins
	write_hw_u32((void *)port, GPIO_DEN, old_enable | pin_mask);
}

static inline void gpio_enable_digital_pin(Port port, uint8_t pin) {
	// record the old enable register value
	uint32_t old_enable = read_hw_u32((void *)port, GPIO_DEN);
	// set this pin's bit
	write_hw_u32((void *)port, GPIO_DEN, old_enable | (1 << pin));
}

/*
gpio_disable_digital_pin_group:
-------------------------------
disables a group of pins

Parameters:
===========
port: the port the pins belong to
pin_mask: mask representing pins to disable

Preconditions
=============
- Requires that the pins disabled are set up as digital pins (in or out)
*/
static inline void gpio_disable_digital_pin_group(Port port, uint8_t pin_mask) {
	// record the old enable register value
	uint32_t old_enable = read_hw_u32((void *)port, GPIO_DEN);
	// mask out the bits for these pins
	write_hw_u32((void *)port, GPIO_DEN, old_enable & ~pin_mask);
}

static inline void gpio_disable_digital_pin(Port port, uint8_t pin) {
	// record the old enable register value
	uint32_t old_enable = read_hw_u32((void *)port, GPIO_DEN);
	// mask out the bits for this pin
	write_hw_u32((void *)port, GPIO_DEN, old_enable & ~(1 << pin));
}

/*
gpio_set_digital_output_pin_group:
----------------------------------
sets the values of a group of pins

Parameters:
===========
port: the port the pins belong to
pin_mask: mask representing pins to enable
pin_states: mask represeting pin values to set

Preconditions
=============
- Requires that the pins enabled are set up as digital output pins
*/
static inline void gpio_set_digital_output_pin_group(Port port, uint8_t pin_mask, uint8_t pin_states) {
	// compute the masking address for these pins
	uintptr_t pin_mask_addr = ((uintptr_t) pin_mask) << 2;
	// write the pins' values
	write_hw_u8((void *)port, GPIO_DATA + pin_mask_addr, pin_states);
}

/*
gpio_set_digital_output_pin:
----------------------------
sets the value of a pin

Parameters:
===========
port: the port the pin belongs to
pin: the pin to read
state: the state to set the pin to

Preconditions
=============
- Requires that the pin is set up as a digital output pin
*/
static inline void gpio_set_digital_output_pin(Port port, uint8_t pin, bool state) {
	// use the group method to limit code duplication.
	uint8_t pin_mask = 1 << pin;
	gpio_set_digital_output_pin_group(port, pin_mask, state ? pin_mask : 0);
}

/*
gpio_get_digital_pin_group:
-----------------------------------
reads the values of a group of pins

Parameters:
===========
port: the port the pins belong to
pin_mask: mask representing pins to read

Preconditions
=============
- Requires that the pins read are set up as digital pins (in or out)
*/
static inline uint8_t gpio_get_digital_pin_group(Port port, uint8_t pin_mask) {
	// compute the masking address for these pins
	uintptr_t pin_mask_addr = ((uintptr_t) pin_mask) << 2;
	// read the pins' values
	return read_hw_u8((void *)port, GPIO_DATA + pin_mask_addr);
}

/*
gpio_get_digital_pin:
------------------------
reads the value of a pin

Parameters:
===========
port: the port the pin belongs to
pin: the pin to read

Preconditions
=============
- Requires that the pin is set up as a digital pin (in or out)
*/
static inline bool gpio_get_digital_pin(Port port, uint8_t pin) {
	// use the group method to limit code duplication.
	uint8_t pin_mask = 1 << pin;
	return gpio_get_digital_pin_group(port, pin_mask);
}

/*
gpio_setup_digital_input_pin_group:
----------------------------------------------
Configures a set of pins to be digital inputs

Parameters
==========
port: port of pin to configure
pin_mask: mask representing pins to configure
drive: the drive mode for these pin

Preconditions
=============
- Requires that the port is enabled (gpio_enable_port)
*/
static inline void gpio_setup_digital_input_pin_group(Port port, uint8_t pin_mask, PinDrive drive) {
	// record old direction and afsel/amsel register values
	uint32_t old_dir = read_hw_u32((void *) port, GPIO_DIR);
	uint32_t old_afsel = read_hw_u32((void *) port, GPIO_AFSEL);
	uint32_t old_amsel = read_hw_u32((void *) port, GPIO_AMSEL);
	// set output direction for these pins
	write_hw_u32((void *) port, GPIO_DIR, old_dir & ~pin_mask);
	// disable alternate-function mode for these pins
	write_hw_u32((void *) port, GPIO_AFSEL, old_afsel & ~pin_mask);
	// enable the analog isolation for these pins
	write_hw_u32((void *) port, GPIO_AMSEL, old_amsel & ~pin_mask);
	
	// read the old 2ma current register of a given type,
	// then write it back with this pin enabled for that current
	// selected by <current>
	uint32_t old_current = read_hw_u32((void *) port, GPIO_DR2R);
	write_hw_u32((void *) port, GPIO_DR2R, old_current | pin_mask);
	
	// It's faster to just unlock the pin always than to check for special pins
	// so unlock the gpio lock in case this pin is a locked pin
	uint32_t old_lock = read_hw_u32((void *) port, GPIO_LOCK);
	write_hw_u32((void *) port, GPIO_LOCK, GPIO_LOCK_KEYVAL);
	// and permit write to the pin via the GPIO_CR register
	uint32_t old_cr = read_hw_u32((void *) port, GPIO_CR);
	write_hw_u32((void *) port, GPIO_CR, old_cr | pin_mask);
	
	// store the old drive register(s) value(s) then write it back with these
	// pins enabled (or disabled) as required
	uint32_t old_drive = 0;
	switch (drive) {
		case kPinDrive_PullUp:
			old_drive = read_hw_u32((void *) port, GPIO_PUR);
			write_hw_u32((void *) port, GPIO_PUR, old_drive | pin_mask);
			break;
			
		case kPinDrive_PullDown:
			old_drive = read_hw_u32((void *) port, GPIO_PDR);
			write_hw_u32((void *) port, GPIO_PDR, old_drive | pin_mask);
			break;
		
		case kPinDrive_OpenDrain:
		default:
			old_drive = read_hw_u32((void *) port, GPIO_PUR);
			write_hw_u32((void *) port, GPIO_PUR, old_drive & ~pin_mask);
			old_drive = read_hw_u32((void *) port, GPIO_PDR);
			write_hw_u32((void *) port, GPIO_PDR, old_drive & ~pin_mask);
			break;
	}
	
	// restore the CR state now that we're done modifying the pins' config
	write_hw_u32((void *) port, GPIO_CR, old_cr);
	// restore the lock state now that we're done with CR
	if (old_lock == 1) {
		write_hw_u32((void *) port, GPIO_LOCK, 0);
	}
}

/*
gpio_setup_digital_input_pin:
---------------------------------------
Configures a pin to be a digital inputs

Parameters
==========
port: port of pin to configure
pin: pin to configure
drive: the drive mode for these pins

Preconditions
=============
- Requires that the port is enabled (gpio_enable_port)
*/
static inline void gpio_setup_digital_input_pin(Port port, uint8_t pin, PinDrive drive) {
	uint32_t pin_mask = 1 << pin;
	gpio_setup_digital_input_pin_group(port, pin_mask, drive);
}

/*
gpio_set_digital_input_pin_group_interrupt_type
---------------------------------------------------------
Configures a group of pins to have a given interrupt type

Parameters
==========
port: port of pins to configure
pin_mask: mask of pins to configure
type: the type of interrupt for these pins
*/
static inline void gpio_set_digital_input_pin_group_interrupt_type(Port port, uint8_t pin_mask, PinInterruptType type) {
	bool int_sense = false;
	bool both_edges = false;
	bool int_event = false;
	switch (type) {
		case kPinInterruptType_BothEdges:
			both_edges = true;
			break;
		case kPinInterruptType_HighLevel:
			int_sense = true;
			int_event = true;
			break;
		case kPinInterruptType_LowLevel:
			int_sense = true;
			break;
		
		case kPinInterruptType_RisingEdge:
			int_event = false;
			break;
		case kPinInterruptType_FallingEdge:
		default:
			break;
	}
	uint32_t int_sense_old = read_hw_u32((void *) port, GPIO_IS);
	write_hw_u32((void *) port, GPIO_IS, (int_sense_old & ~pin_mask) | (int_sense ? pin_mask : 0));
	uint32_t int_event_old = read_hw_u32((void *) port, GPIO_IEV);
	write_hw_u32((void *) port, GPIO_IEV, (int_event_old & pin_mask) | (int_event ? pin_mask : 0));
	uint32_t both_edges_old = read_hw_u32((void *) port, GPIO_IBE);
	write_hw_u32((void *) port, GPIO_IBE, (both_edges_old & pin_mask) | (both_edges ? pin_mask : 0));
}

/*
gpio_set_digital_input_pin_group_interrupt_type
-----------------------------------------------
Configures a pin to have a given interrupt type

Parameters
==========
port: port of pin to configure
pin: pin to configure
type: the type of interrupt for these pins
*/
static inline void gpio_set_digital_input_pin_interrupt_type(Port port, uint8_t pin, PinInterruptType type) {
	uint8_t bit = 1 << pin;
	gpio_set_digital_input_pin_group_interrupt_type(port, bit, type);
}

/*
gpio_digital_pin_group_disable_interrupts
-----------------------------------------
Disables interrupts for a group of pins

Parameters
==========
port: port of pins to configure
pin_mask: mask of pins to configure
*/
static inline void gpio_digital_pin_group_disable_interrupts(Port port, uint8_t pin_mask) {
	uint32_t int_mask_old = read_hw_u32((void *) port, GPIO_IM);
	write_hw_u32((void *) port, GPIO_IM, int_mask_old & ~pin_mask);
}

/*
gpio_digital_pin_disable_interrupts
-----------------------------------
Disables interrupts for a pin

Parameters
==========
port: port of pins to configure
pin: pin to configure
*/
static inline void gpio_digital_pin_disable_interrupts(Port port, uint8_t pin) {
	uint8_t bit = 1 << pin;
	gpio_digital_pin_group_disable_interrupts(port, bit);
}

/*
gpio_digital_pin_group_enable_interrupts
----------------------------------------
Enables interrupts for a group of pins

Parameters
==========
port: port of pins to configure
pin_mask: mask of pins to configure
*/
static inline void gpio_digital_pin_group_enable_interrupts(Port port, uint8_t pin_mask) {
	uint32_t int_mask_old = read_hw_u32((void *) port, GPIO_IM);
	write_hw_u32((void *) port, GPIO_IM, int_mask_old | pin_mask);
	if (pin_mask != 0) {
		set_isr_enabled(__gpio_port_to_interrupt_handler(port));
	}
}

/*
gpio_digital_pin_enable_interrupts
----------------------------------
Enables interrupts for a pin

Parameters
==========
port: port of pins to configure
pin: pin to configure
*/
static inline void gpio_digital_pin_enable_interrupts(Port port, uint8_t pin) {
	uint8_t bit = 1 << pin;
	gpio_digital_pin_group_enable_interrupts(port, bit);
}

/*
gpio_set_interrupt_handler
------------------------------------------------
Sets the appropriate ISR for the given GPIO port

Parameters
==========
port: port generating interrupts
handler: pointer to interrupt handler function
*/
static inline void gpio_set_interrupt_handler(Port port, void (*handler)()) {
	set_isr(__gpio_port_to_interrupt_handler(port), handler);
}

/*
gpio_enable_port_interrupts
---------------------------------
Enables interrupts on a GPIO port

Parameters
==========
port: the port that will generate interrupts
*/
static inline void gpio_enable_port_interrupts(Port port) {
	set_isr_enabled(__gpio_port_to_interrupt_handler(port));
}

/*
gpio_disable_port_interrupts
----------------------------------
Disables interrupts on a GPIO port

Parameters
==========
port: the port that will generate interrupts
*/
static inline void gpio_disable_port_interrupts(Port port) {
	set_isr_disabled(__gpio_port_to_interrupt_handler(port));
}

/*
gpio_get_interrupt_status
----------------------------------------
Gets the interrupt status of a GPIO port

Parameters
==========
port: the port to query

Returns
=======
A bitfield where each one represents that the corresponding pin
is signaling an interrupt condition
*/
static inline uint8_t gpio_get_interrupt_status(Port port) {
	return read_hw_u32((void *) port, GPIO_MIS) & 0xFF;
}

/*
gpio_digital_input_pin_group_clear_interrupt
--------------------------------------------------
Clears the interrupt status of pins of a GPIO port

Parameters
==========
port: the port to clear
pin_mask: the mask of pins to clear
*/
static inline void gpio_digital_input_pin_group_clear_interrupt(Port port, uint8_t pin_mask) {
	uint32_t status = gpio_get_interrupt_status(kPortF);
	uint32_t old_cir = read_hw_u32((void *) port, GPIO_ICR);
	write_hw_u32((void *) port, GPIO_ICR, (old_cir & 0xFFFFFF00) | (pin_mask & status));
}

/*
gpio_setup_digital_pin_group_alternate_function
------------------------------------------------------------
Sets up a group of digital pins to use an alternate function

Parameters
==========
port: the port of the pins to configure
pin_mask: the mask of the pins to configure
function: the function to assign to the pins
output: whether or not these pins are in output mode
current: how much current can these pins source/sink
drive: the drive mode of the pin
*/
static inline void gpio_setup_digital_pin_group_alternate_function(Port port, uint8_t pin_mask, uint8_t function, bool output, DriveCurrent current, PinDrive drive) {
	// record old direction and afsel register values
	uint32_t old_dir = read_hw_u32((void *)port, GPIO_DIR);
	uint32_t old_afsel = read_hw_u32((void *)port, GPIO_AFSEL);
	uint32_t old_amsel = read_hw_u32((void *) port, GPIO_AMSEL);
	// set output direction for these pins
	write_hw_u32((void *)port, GPIO_DIR, output ? (old_dir | pin_mask) : (old_dir & ~ pin_mask));
	// disable alternate-function mode for this pin
	write_hw_u32((void *)port, GPIO_AFSEL, old_afsel | pin_mask);
	// enable the analog isolation for these pins
	write_hw_u32((void *) port, GPIO_AMSEL, old_amsel & ~pin_mask);
	// set the port control value for each pin
	uint32_t pctl = read_hw_u32((void *) port, GPIO_PCTL);
	for (int i = 0; i < 8; i ++) {
		if (pin_mask & (1 << i)) {
			pctl &= ~ (0xF << (i << 2));
			pctl |= ((function & 0x0F) << (i << 2));
		}
	}
	write_hw_u32((void *) port, GPIO_PCTL, pctl);
	// read the old current register of a given type,
	// then write it back with this pin enabled for the current
	// selected by <current>
	uint32_t old_current = 0;
	switch (current) {
		case kDriveCurrent_12ma:
			old_current = read_hw_u32((void *)port, GPIO_DR12R);
			write_hw_u32((void *)port, GPIO_DR12R, old_current | pin_mask);
			break;
		
		case kDriveCurrent_8ma:
			old_current = read_hw_u32((void *)port, GPIO_DR8R);
			write_hw_u32((void *)port, GPIO_DR8R, old_current | pin_mask);
			break;
			
		case kDriveCurrent_4ma:
			old_current = read_hw_u32((void *)port, GPIO_DR4R);
			write_hw_u32((void *)port, GPIO_DR4R, old_current | pin_mask);
			break;
			
		case kDriveCurrent_2ma:
		default:
			old_current = read_hw_u32((void *)port, GPIO_DR2R);
			write_hw_u32((void *)port, GPIO_DR2R, old_current | pin_mask);
			break;
	}
	// It's faster to just unlock the pin always than check for special pins
	// so unlock the gpio lock in case this pin is a locked pin
	uint32_t old_lock = read_hw_u32((void *)port, GPIO_LOCK);
	write_hw_u32((void *)port, GPIO_LOCK, GPIO_LOCK_KEYVAL);
	// and permit write to the pin via the GPIO_CR register
	uint32_t old_cr = read_hw_u32((void *)port, GPIO_CR);
	write_hw_u32((void *)port, GPIO_CR, old_cr | pin_mask);
	
	// store the old drive register(s) value(s) then write it back with these
	// pins enabled (or disabled) as required
	uint32_t old_drive = 0;
	switch (drive) {
		case kPinDrive_PullUp:
			old_drive = read_hw_u32((void *)port, GPIO_PUR);
			write_hw_u32((void *)port, GPIO_PUR, old_drive | pin_mask);
			break;
			
		case kPinDrive_PullDown:
			old_drive = read_hw_u32((void *)port, GPIO_PDR);
			write_hw_u32((void *)port, GPIO_PDR, old_drive | pin_mask);
			break;
		
		case kPinDrive_OpenDrain:
		default:
			old_drive = read_hw_u32((void *)port, GPIO_PUR);
			write_hw_u32((void *)port, GPIO_PUR, old_drive & ~pin_mask);
			old_drive = read_hw_u32((void *)port, GPIO_PDR);
			write_hw_u32((void *)port, GPIO_PDR, old_drive & ~pin_mask);
			break;
	}
	
	// restore the CR state now that we're done modifying the pins' config
	write_hw_u32((void *)port, GPIO_CR, old_cr);
	// restore the lock state now that we're done with CR
	if (old_lock == 1) {
		write_hw_u32((void *)port, GPIO_LOCK, 0);
	}
}

/*
gpio_setup_analog_input_pin_group
------------------------------------------
Sets up GPIO pins for use as analog inputs

Parameters
==========
port: the port of the pins to configure
pin_mask: mask of pins to configure
*/
static inline void gpio_setup_analog_input_pin_group(Port port, uint8_t pin_mask) {
	gpio_disable_digital_pin_group(port, pin_mask);
}

/*
gpio_enable_analog_input_pin_group
-------------------------------------
Enables up GPIO pins for analog input

Parameters
==========
port: the port of the pins to enable
pin_mask: mask of pins to enable
*/
static inline void gpio_enable_analog_input_pin_group(Port port, uint8_t pin_mask) {
	uint32_t old_amsel = read_hw_u32((void *) port, GPIO_AMSEL);
	// disable the analog isolation for these pins
	write_hw_u32((void *) port, GPIO_AMSEL, old_amsel | pin_mask);
}

/*
gpio_disable_analog_input_pin_group
--------------------------------------
Disables up GPIO pins for analog input

Parameters
==========
port: the port of the pins to disable
pin_mask: mask of pins to disable
*/
static inline void gpio_disable_analog_input_pin_group(Port port, uint8_t pin_mask) {
	uint32_t old_amsel = read_hw_u32((void *) port, GPIO_AMSEL);
	// enable the analog isolation for these pins
	write_hw_u32((void *) port, GPIO_AMSEL, old_amsel & ~ pin_mask);
}

#endif
