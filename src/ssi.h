/*
Liam Taylor
wttesla@uw.edu

ssi.h:
TM4C123GH6PM SSI peripheral driver
==================================
Contains routines for configuring and using the Synchonous Serial Interface
*/

#ifndef SSI_H
#define SSI_H

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "gpio.h"

typedef enum {
	kSSI0 = 0,
	kSSI1_PortD = 1,
	kSSI2 = 2,
	kSSI3 = 3,
	kSSI1_PortF = 4
} SSI;

static void * _ssi_base_addresses[] = {
	[kSSI0] = (void *) SSI_SSI0_BASE,
	[kSSI1_PortD] = (void *) SSI_SSI1_BASE,
	[kSSI2] = (void *) SSI_SSI2_BASE,
	[kSSI3] = (void *) SSI_SSI3_BASE,
	[kSSI1_PortF] = (void *) SSI_SSI1_BASE,
};

typedef struct {
	uint8_t clock;
	uint8_t frame_sig;
	uint8_t rx;
	uint8_t tx;
	Port port;
	uint8_t function;
} SSIPinAssignments;

/*
SSI PIN ASSIGNMENTS
===================
*/

static SSIPinAssignments _ssi_pins[] = {
	[kSSI0] = {.clock = 2, .frame_sig = 3, .rx = 4, .tx = 5, .port = kPortA, .function = 2},
	[kSSI1_PortD] = {.clock = 0, .frame_sig = 1, .rx = 2, .tx = 3, .port = kPortD, .function = 2},
	[kSSI2] = {.clock = 4, .frame_sig = 5, .rx = 6, .tx = 7, .port = kPortB, .function = 2},
	[kSSI3] = {.clock = 0, .frame_sig = 1, .rx = 2, .tx = 3, .port = kPortD, .function = 1},
	[kSSI1_PortF] = {.clock = 2, .frame_sig = 3, .rx = 0, .tx = 1, .port = kPortF, .function = 2},
};

static const uint8_t kSSIPinType_Clock = 1;
static const uint8_t kSSIPinType_FrameSignal = 2;
static const uint8_t kSSIPinType_TX = 4;
static const uint8_t kSSIPinType_RX = 8;

typedef enum {
	kSSIFrameFormat_FreescaleSPI_FirstEdge = SSI_CR0_FRAME_FORMAT_SPI,
	kSSIFrameFormat_FreescaleSPI_SecondEdge = SSI_CR0_FRAME_FORMAT_SPI | SSI_CR0_SERIAL_CLOCK_PHASE_SECOND_CLOCK_EDGE,
	kSSIFrameFormat_TISynchronousSerial = SSI_CR0_FRAME_FORMAT_TI_SSFF,
	kSSIFrameFormat_MICROWIRE = SSI_CR0_FRAME_FORMAT_MICROWIRE
} SSIFrameFormat;

/*
ssi_enable
------------------------------
Enables a given SSI peripheral

Parameters
==========
ssi: the specific ssi peripheral to enable
*/
static inline void ssi_enable(SSI ssi) {
	uint32_t rcgcssi_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCSSI);
	if (ssi == kSSI1_PortF)
		ssi = kSSI1_PortD;
	uint32_t bit = 1 << ssi;
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCSSI, rcgcssi_old | bit);
	gpio_enable_port(_ssi_pins[ssi].port);
}

/*
ssi_stop
------------------------------------------------------------
Stops a given SSI peripheral from transmitting and receiving

Parameters
==========
ssi: the specific ssi peripheral to stop
*/
static inline void ssi_stop(SSI ssi) {
	uint32_t cr1_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1);
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1, cr1_old & ~ SSI_CR1_SSE);
}

/*
ssi_start
--------------------------------------------------------
Starts a given SSI peripheral transmitting and receiving

Parameters
==========
ssi: the specific ssi peripheral to stop
*/
static inline void ssi_start(SSI ssi) {
	uint32_t cr1_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1);
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1, cr1_old | SSI_CR1_SSE);
}

/*
ssi_setup
----------------------------
Configures an SSI peripheral

Parameters
==========
ssi: the specific ssi to configure
pin_types: a mask of the pins to use and enable for the SSI by type, (kSSIPinType_Clock, kSSIPinType_FrameSignal, kSSIPinType_RX, kSSIPinType_TX)
format: the type of serial bus to implement with the SSI (kSSIFrameFormat_FreescaleSPI_FirstEdge, kSSIFrameFormat_FreescaleSPI_SecondEdge, kSSIFrameFormat_TISynchronousSerial, kSSIFrameFormat_MICROWIRE)
master: is this port a bus master or bus slave
main_clock: whether or not to use the main clock or the precision internal oscillator
clock_rate: how many clock cycles per serial bit transmitted?
prescaler: the prescaler for the clock
data_size: the size of each data byte (2-16)
clock_high: whether or not the lock is active-high
*/
static inline void ssi_setup(SSI ssi, uint8_t pin_types, SSIFrameFormat format, bool master, bool main_clock, uint8_t clock_rate, uint8_t prescaler, uint8_t data_size, bool clock_high, DriveCurrent drive_current, PinDrive pin_drive) {
	if (pin_types & kSSIPinType_Clock) {
		gpio_setup_digital_pin_group_alternate_function(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].clock, _ssi_pins[ssi].function, master, drive_current, pin_drive);
		gpio_enable_digital_pin_group(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].clock);
	}
	if (pin_types & kSSIPinType_FrameSignal) {
		gpio_setup_digital_pin_group_alternate_function(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].frame_sig, _ssi_pins[ssi].function, master, drive_current, pin_drive);
		gpio_enable_digital_pin_group(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].frame_sig);
	}
	if (pin_types & kSSIPinType_RX) {
		gpio_setup_digital_pin_group_alternate_function(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].rx, _ssi_pins[ssi].function, false, drive_current, pin_drive);
		gpio_enable_digital_pin_group(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].rx);
	}
	if (pin_types & kSSIPinType_TX) {
		gpio_setup_digital_pin_group_alternate_function(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].tx, _ssi_pins[ssi].function, true, drive_current, pin_drive);
		gpio_enable_digital_pin_group(_ssi_pins[ssi].port, 1 << _ssi_pins[ssi].tx);
	}
	
	uint32_t cr1_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1);
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR1, (cr1_old & ~ SSI_CR1_MS) | (master ? 0 : SSI_CR1_MS));
	uint32_t cc_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CC);
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CC, (cc_old & ~ SSI_CC_CLOCK_MASK) | (main_clock ? SSI_CC_CLOCK_SYSTEM : SSI_CC_CLOCK_PIOSC));
	uint32_t cpsr_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CPSR);
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CPSR, (cpsr_old & ~ SSI_CPSR_MASK) | ((uint32_t) prescaler));
	uint32_t cr0_old = read_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR0);
	cr0_old &= ~(SSI_CR0_DATA_SIZE_MASK | SSI_CR0_FRAME_FORMAT_MASK | SSI_CR0_SERIAL_CLOCK_POLARITY_HIGH | SSI_CR0_SERIAL_CLOCK_PHASE_SECOND_CLOCK_EDGE | SSI_CR0_SERIAL_CLOCK_RATE_MASK);
	cr0_old |= ((data_size - 1) & SSI_CR0_DATA_SIZE_MASK);
	cr0_old |= (uint32_t) format;
	cr0_old |= clock_high ? SSI_CR0_SERIAL_CLOCK_POLARITY_HIGH : 0;
	cr0_old |= (clock_rate << SSI_CR0_SERIAL_CLOCK_RATE_SHIFT) & SSI_CR0_SERIAL_CLOCK_RATE_MASK;
	write_hw_u32((void *) _ssi_base_addresses[ssi], SSI_CR0, cr0_old);
}

/*
ssi_transmit
--------------------------------
Send a byte of data over the SSI

Parameters
==========
ssi: the specific SSI to use
byte_value: the value, right-justified, to push to the FIFO transmit queue
*/
static inline void ssi_transmit(SSI ssi, uint16_t byte_value) {
	write_hw_u16(_ssi_base_addresses[ssi], SSI_DATA, byte_value);
}

/*
ssi_transmit_full
----------------------------------------------
Checks whether the transmit FIFO queue is full

Parameters
==========
ssi: the specific SSI to use

Return
======
a boolean value indicating if the transmit FIFO queue is full
*/
static inline bool ssi_transmit_full(SSI ssi) {
	return ! (read_hw_u32(_ssi_base_addresses[ssi], SSI_STATUS) & SSI_STATUS_TRANSMIT_NOT_FULL);
}

/*
ssi_transmit_empty
-----------------------------------------------
Checks whether the transmit FIFO queue is empty

Parameters
==========
ssi: the specific SSI to use

Return
======
a boolean value indicating if the transmit FIFO queue is empty
*/
static inline bool ssi_transmit_empty(SSI ssi) {
	return !! (read_hw_u32(_ssi_base_addresses[ssi], SSI_STATUS) & SSI_STATUS_TRANSMIT_EMPTY);
}

#endif
