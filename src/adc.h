/*
Liam Taylor
wttesla@uw.edu

adc.h:
TM4C123GH6PM ADC driver
=======================
Contains routines for configuring and using the ADC modules
*/

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "gpio.h"
#include "isr.h"

typedef enum {
	kADC1 = 0,
	kADC2 = 1,
} ADC;

typedef enum {
	kAnalogInput_PE3 = 0,
	kAnalogInput_PE2 = 1,
	kAnalogInput_PE1 = 2,
	kAnalogInput_PE0 = 3,
	kAnalogInput_PD3 = 4,
	kAnalogInput_PD2 = 5,
	kAnalogInput_PD1 = 6,
	kAnalogInput_PD0 = 7,
	kAnalogInput_PE5 = 8,
	kAnalogInput_PE4 = 9,
	kAnalogInput_PB4 = 10,
	kAnalogInput_PB5 = 11,
	kAnalogInput_Temperature = 12,
} AnalogInput;

typedef enum {
	kADCSampler0 = 0,
	kADCSampler1 = 1,
	kADCSampler2 = 2,
	kADCSampler3 = 3,
} ADCSampler;

typedef enum {
	kADCSamplerPriority_0123 = ADC_SSPRI_0123,
	kADCSamplerPriority_0132 = ADC_SSPRI_0132,
	kADCSamplerPriority_0213 = ADC_SSPRI_0213,
	kADCSamplerPriority_0231 = ADC_SSPRI_0231,
	kADCSamplerPriority_0321 = ADC_SSPRI_0321,
	kADCSamplerPriority_0312 = ADC_SSPRI_0312,
	kADCSamplerPriority_1023 = ADC_SSPRI_1023,
	kADCSamplerPriority_1032 = ADC_SSPRI_1032,
	kADCSamplerPriority_1203 = ADC_SSPRI_1203,
	kADCSamplerPriority_1230 = ADC_SSPRI_1230,
	kADCSamplerPriority_1302 = ADC_SSPRI_1302,
	kADCSamplerPriority_1320 = ADC_SSPRI_1320,
	kADCSamplerPriority_2013 = ADC_SSPRI_2013,
	kADCSamplerPriority_2031 = ADC_SSPRI_2031,
	kADCSamplerPriority_2103 = ADC_SSPRI_2103,
	kADCSamplerPriority_2130 = ADC_SSPRI_2130,
	kADCSamplerPriority_2301 = ADC_SSPRI_2301,
	kADCSamplerPriority_2310 = ADC_SSPRI_2310,
	kADCSamplerPriority_3012 = ADC_SSPRI_3012,
	kADCSamplerPriority_3021 = ADC_SSPRI_3021,
	kADCSamplerPriority_3102 = ADC_SSPRI_3102,
	kADCSamplerPriority_3120 = ADC_SSPRI_3120,
	kADCSamplerPriority_3210 = ADC_SSPRI_3210,
	kADCSamplerPriority_3201 = ADC_SSPRI_3201,
} ADCSamplerPriority;

typedef struct {
	void * base;
	uint32_t ss_interrupts[4];
} ADCInfo;

typedef struct {
	uint32_t emux_mask;
	uint32_t emux_shift;
	uint32_t max_samples;
	uintptr_t ssmux_offset;
	uintptr_t ssctl_offset;
	uintptr_t ssfifo_offset;
	uintptr_t ssfstat_offset;
} ADCSamplerInfo;

typedef struct {
	Port port;
	uint8_t pin;
	bool temp;
} AnalogInputInfo;

typedef struct {
	AnalogInput input;
	bool interrupt;
	bool differential;
} ADCSample;

static ADCInfo __adc_info[] = {
	[kADC1] = {.base = (void *) ADC_ADC1_BASE, .ss_interrupts = {INTR_ADC1_SS0, INTR_ADC1_SS1, INTR_ADC1_SS2, INTR_ADC1_SS3}},
	[kADC2] = {.base = (void *) ADC_ADC2_BASE, .ss_interrupts = {INTR_ADC2_SS0, INTR_ADC2_SS1, INTR_ADC2_SS2, INTR_ADC2_SS3}},
};

static AnalogInputInfo __analog_input_info[] = {
	[kAnalogInput_PE3] = {.port = kPortE, .pin = 3, .temp = false},
	[kAnalogInput_PE2] = {.port = kPortE, .pin = 2, .temp = false},
	[kAnalogInput_PE1] = {.port = kPortE, .pin = 1, .temp = false},
	[kAnalogInput_PE0] = {.port = kPortE, .pin = 0, .temp = false},
	[kAnalogInput_PD3] = {.port = kPortD, .pin = 3, .temp = false},
	[kAnalogInput_PD2] = {.port = kPortD, .pin = 2, .temp = false},
	[kAnalogInput_PD1] = {.port = kPortD, .pin = 1, .temp = false},
	[kAnalogInput_PD0] = {.port = kPortD, .pin = 0, .temp = false},
	[kAnalogInput_PE5] = {.port = kPortE, .pin = 5, .temp = false},
	[kAnalogInput_PE4] = {.port = kPortE, .pin = 4, .temp = false},
	[kAnalogInput_PB4] = {.port = kPortB, .pin = 4, .temp = false},
	[kAnalogInput_PB5] = {.port = kPortB, .pin = 5, .temp = false},
	[kAnalogInput_Temperature] = {.port = kPortF, .pin = 9, .temp = true},
};

static ADCSamplerInfo __adc_sampler_info[] = {
	[kADCSampler0] = {.emux_mask = ADC_EMUX_SAMPLER0_MASK, .emux_shift = 0, .max_samples = 8, .ssmux_offset = ADC_SSMUX0, .ssctl_offset = ADC_SSCTL0, .ssfstat_offset = ADC_SSFSTAT0, .ssfifo_offset = ADC_SSFIFO0},
	[kADCSampler1] = {.emux_mask = ADC_EMUX_SAMPLER1_MASK, .emux_shift = 4, .max_samples = 4, .ssmux_offset = ADC_SSMUX1, .ssctl_offset = ADC_SSCTL1, .ssfstat_offset = ADC_SSFSTAT1, .ssfifo_offset = ADC_SSFIFO1},
	[kADCSampler2] = {.emux_mask = ADC_EMUX_SAMPLER2_MASK, .emux_shift = 8, .max_samples = 4, .ssmux_offset = ADC_SSMUX2, .ssctl_offset = ADC_SSCTL2, .ssfstat_offset = ADC_SSFSTAT2, .ssfifo_offset = ADC_SSFIFO2},
	[kADCSampler3] = {.emux_mask = ADC_EMUX_SAMPLER3_MASK, .emux_shift = 12, .max_samples = 1, .ssmux_offset = ADC_SSMUX3, .ssctl_offset = ADC_SSCTL3, .ssfstat_offset = ADC_SSFSTAT3, .ssfifo_offset = ADC_SSFIFO3},
};

/*
adc_enable
--------------
Enables an ADC

Parameters
==========
adc: which adc to enable
*/
static inline void adc_enable(ADC adc) {
	uint32_t rcgc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCADC);
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCADC, rcgc_old | (1 << adc));
	for (volatile uint32_t i = 0; i < 3; i ++);
}

/*
adc_set_sampler_priority
------------------------------------------
Sets the sampler priority order for an adc

Parameters
==========
adc: which adc to configure
priority: which priority order to use (kADCSamplerPriority_*)
*/
static inline void adc_set_sampler_priority(ADC adc, ADCSamplerPriority priority) {
	uint32_t sspri_old = read_hw_u32(__adc_info[adc].base, ADC_SSPRI);
	write_hw_u32(__adc_info[adc].base, ADC_SSPRI, (sspri_old & ~ ADC_SSPRI_MASK) | priority);
}

/*
adc_sampler_disable
----------------------------
Disables a sampler of an ADC

Parameters
==========
adc: which adc to configure
sampler: which sampler to disable
*/
static inline void adc_sampler_disable(ADC adc, ADCSampler sampler) {
	uint32_t actss_old = read_hw_u32(__adc_info[adc].base, ADC_ACTSS);
	write_hw_u32(__adc_info[adc].base, ADC_ACTSS, actss_old & ~ (1 << sampler));
}

/*
adc_sampler_enable
----------------------------
Disables a sampler of an ADC

Parameters
==========
adc: which adc to configure
sampler: which sampler to enable
*/

static inline void adc_sampler_enable(ADC adc, ADCSampler sampler) {
	uint32_t actss_old = read_hw_u32(__adc_info[adc].base, ADC_ACTSS);
	uint32_t actss = actss_old | (1 << sampler);
	write_hw_u32(__adc_info[adc].base, ADC_ACTSS, actss);
}

/*
adc_sampler_setup_trigger_cpu
---------------------------------------
Set an adc sampler to be CPU triggered.

Parameters
==========
adc: which adc to configure
sampler: which sampler to configure
*/
static inline void adc_sampler_setup_trigger_cpu(ADC adc, ADCSampler sampler) {
	adc_sampler_disable(adc, sampler);
	uint32_t emux_old = read_hw_u32(__adc_info[adc].base, ADC_EMUX);
	write_hw_u32(__adc_info[adc].base, ADC_EMUX, (emux_old & ~ (__adc_sampler_info[sampler].emux_mask)) | (ADC_EMUX_ESOURCE_CPU << __adc_sampler_info[sampler].emux_shift));
}

/*
adc_sampler_setup_trigger_timer
----------------------------------------
Set an adc sampler to be Timer triggered

Parameters
==========
adc: which adc to configure
sampler: which sampler to configure
*/
static inline void adc_sampler_setup_trigger_timer(ADC adc, ADCSampler sampler) {
	adc_sampler_disable(adc, sampler);
	uint32_t emux_old = read_hw_u32(__adc_info[adc].base, ADC_EMUX);
	write_hw_u32(__adc_info[adc].base, ADC_EMUX, (emux_old & ~ (__adc_sampler_info[sampler].emux_mask)) | (ADC_EMUX_ESOURCE_TIMER << __adc_sampler_info[sampler].emux_shift));
}

/*
adc_sampler_setup_inputs
--------------------------------------------------
Configures the sampling schedule for this sampler.

Parameters
==========
adc: which adc to configure
sampler: which sampler to configure
input_list: array of ADCSample structs describing each sample
input_count: number of samples in input_list
*/
static inline void adc_sampler_setup_inputs(ADC adc, ADCSampler sampler, ADCSample * input_list, uint32_t input_count) {
	if (input_count > __adc_sampler_info[sampler].max_samples)
		input_count = __adc_sampler_info[sampler].max_samples;
	uint32_t ssctl = 0;
	uint32_t ssmux = 0;
	uint32_t i = 0;
	for (; i < input_count; i ++) {
		uint32_t ctl_bits = 0;
		uint32_t mux_bits = 0;
		if (i == (input_count - 1)) {
			ctl_bits |= ADC_SSCTL_END_OF_SEQUENCE;
		}
		if (__analog_input_info[input_list[i].input].temp) {
			ctl_bits |= ADC_SSCTL_TEMP;
			mux_bits = 0;
		} else {
			mux_bits = input_list[i].input;
		}
		if (input_list[i].interrupt) {
			ctl_bits |= ADC_SSCTL_INTERRUPT;
		}
		// only allow differential inputs if they are aligned-by-two pairs
		if (input_list[i].differential && ((!(i & 1) && (i < input_count - 1) && (input_list[i + 1].differential)) || ((i & 1) && input_list[i + 1].differential))) {
			ctl_bits |= ADC_SSCTL_DIFFERENTIAL;
		}
		ssctl |= (ctl_bits << (i * 4));
		ssmux |= (mux_bits << (i * 4));
	}
	uint32_t ssctl_old = read_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssctl_offset);
	uint32_t ssmux_old = read_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssmux_offset);
	for (; i < 8; i ++) {
		ssctl |= ((0xF << (i * 4)) & ssctl_old);
		ssmux |= ((0xF << (i * 4)) & ssmux_old);
	}
	write_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssmux_offset, ssmux);
	write_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssctl_offset, ssctl);
	for (i = 0; i < input_count; i ++) {
		if (! __analog_input_info[input_list[i].input].temp) {
			gpio_enable_port(__analog_input_info[input_list[i].input].port);
			gpio_setup_analog_input_pin_group(__analog_input_info[input_list[i].input].port, 1 << __analog_input_info[input_list[i].input].pin);
			gpio_enable_analog_input_pin_group(__analog_input_info[input_list[i].input].port, 1 << __analog_input_info[input_list[i].input].pin);
		}
	}
}

/*
adc_sampler_enable_interrupts
------------------------------------
Enables interrupts for this sampler.

Parameters
==========
adc: which adc to configure
sampler: which sampler to configure
*/
static inline void adc_sampler_enable_interrupts(ADC adc, ADCSampler sampler) {
	uint32_t im_old = read_hw_u32(__adc_info[adc].base, ADC_IM);
	set_isr_enabled(__adc_info[adc].ss_interrupts[sampler]);
	write_hw_u32(__adc_info[adc].base, ADC_IM, im_old | (1 << sampler));
}

/*
adc_sampler_disable_interrupts
-------------------------------------
Disables interrupts for this sampler.

Parameters
==========
adc: which adc to configure
sampler: which sampler to configure
*/
static inline void adc_sampler_disable_interrupts(ADC adc, ADCSampler sampler) {
	uint32_t im_old = read_hw_u32(__adc_info[adc].base, ADC_IM);
	write_hw_u32(__adc_info[adc].base, ADC_IM, im_old & ~ (1 << sampler));
	set_isr_disabled(__adc_info[adc].ss_interrupts[sampler]);
}

/*
adc_sampler_clear_interrupt
----------------------------------------
Clears interrupt status for this sampler

Parameters
==========
adc: which adc to clear
sampler: which sampler to clear
*/
static inline void adc_sampler_clear_interrupt(ADC adc, ADCSampler sampler) {
	uint32_t isc_old = read_hw_u32(__adc_info[adc].base, ADC_ISC);
	write_hw_u32(__adc_info[adc].base, ADC_ISC, (isc_old & (~0xF)) | (1 << sampler));
}

/*
adc_sampler_get_interrupt_status
--------------------------------------
gets the interrupt status of a sampler

Parameters
==========
adc: the adc containing the sampler
sampler: which sampler check

Returns
=======
true if the sampler is signaling an interrupt
false otherwise
*/
static inline bool adc_sampler_get_interrupt_status(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, ADC_RIS) & (1 << sampler);
}

/*
adc_sampler_set_interrupt_handler
------------------------------------------------
sets the interrupt handler for the given sampler

Parameters
==========
adc: the adc containing the sampler
sampler: which sampler will trigger this andler
handler: the interrupt handler's address
*/
static inline void adc_sampler_set_interrupt_handler(ADC adc, ADCSampler sampler, void (* handler)()) {
	set_isr(__adc_info[adc].ss_interrupts[sampler], handler);
}

/*
adc_poll_busy
-------------------------------------------------------------
returns a boolean representing whether the adc is busy or not

Parameters
----------
adc: the adc to poll

Returns
=======
true if the adc is busy
false otherwise
*/
static inline bool adc_poll_busy(ADC adc) {
	return read_hw_u32(__adc_info[adc].base, ADC_ACTSS) & ADC_ACTSS_ADC_BUSY;
}

/*
adc_sampler_fifo_empty
---------------------------------------------------------------------------
returns a boolean representing whether sampler's fifo queue is empty or not

Parameters
----------
adc: the adc containing the sampler
sampler: the sampler to check

Returns
=======
true if the fifo queue is empty
false otherwise
*/
static inline bool adc_sampler_fifo_empty(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssfstat_offset) & ADC_SSFSTAT_EMPTY;
}

/*
adc_sampler_fifo_full
--------------------------------------------------------------------------
returns a boolean representing whether sampler's fifo queue is full or not

Parameters
----------
adc: the adc containing the sampler
sampler: the sampler to check

Returns
=======
true if the fifo queue is full
false otherwise
*/
static inline bool adc_sampler_fifo_full(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssfstat_offset) & ADC_SSFSTAT_FULL;
}

/*
adc_sampler_fifo_underflow
---------------------------------------------------------------------------
returns a boolean representing whether sampler's fifo queue has underflowed

Parameters
----------
adc: the adc containing the sampler
sampler: the sampler to check

Returns
=======
true if the fifo queue has underflowed
false otherwise
*/
static inline bool adc_sampler_fifo_underflow(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, ADC_USTAT) & (1 << sampler);
}

/*
adc_sampler_fifo_overflow
--------------------------------------------------------------------------
returns a boolean representing whether sampler's fifo queue has overflowed

Parameters
----------
adc: the adc containing the sampler
sampler: the sampler to check

Returns
=======
true if the fifo queue has overflow
false otherwise
*/
static inline bool adc_sampler_fifo_overflow(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, ADC_OSTAT) & (1 << sampler);
}

/*
adc_sampler_fifo_pop
-------------------------------------------------------------------
pops a value off of the sampler fifo.
value is undefined if the fifo is empty

Parameters
==========
adc: the adc containing the sampler
sampler: the sampler who's fifo to pop from
*/
static inline uint32_t adc_sampler_fifo_pop(ADC adc, ADCSampler sampler) {
	return read_hw_u32(__adc_info[adc].base, __adc_sampler_info[sampler].ssfifo_offset) & ADC_SSFIFO_MASK;
}

/*
adc_sampler_trigger
----------------------------------------
triggers the sampler to start conversion

Parameters
==========
adc: the adc containing the sampler
sampler: the sampler to trigger
pend_sync: whether or not to wait for a call to adc_sampler_sync to trigger
*/
static inline void adc_sampler_trigger(ADC adc, ADCSampler sampler, bool pend_sync) {
	uint32_t pssi_old = read_hw_u32(__adc_info[adc].base, ADC_PSSI);
	write_hw_u32(__adc_info[adc].base, ADC_PSSI, (pssi_old & ~ (ADC_PSSI_SSIINIT_MASK | ADC_PSSI_GSYNC | ADC_PSSI_SYCNWAIT)) | (1 << sampler) | (pend_sync ? ADC_PSSI_SYCNWAIT : 0));
}

/*
adc_sync_trigger
------------------------------------------------
trigger all pending samplers to start conversion

Parameters
==========
adc: the adc containing the pending samplers
*/
static inline void adc_sync_trigger(ADC adc) {
	uint32_t pssi_old = read_hw_u32(__adc_info[adc].base, ADC_PSSI);
	write_hw_u32(__adc_info[adc].base, ADC_PSSI, (pssi_old & ~(ADC_PSSI_SSIINIT_MASK | ADC_PSSI_SYCNWAIT)) | ADC_PSSI_GSYNC);
}

static inline uint32_t adc_temp_celcius_thousandths_from_value(uint32_t value) {
	return 147500 - ((75 * 3300 * value) / 4096);
}

#endif
