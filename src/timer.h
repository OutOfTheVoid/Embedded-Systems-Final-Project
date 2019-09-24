#ifndef TIMER_H
#define TIMER_H

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "isr.h"

typedef enum {
	kTimer0 = GPTM_TIMER0_BASE,
	kTimer1 = GPTM_TIMER1_BASE,
	kTimer2 = GPTM_TIMER2_BASE,
	kTimer3 = GPTM_TIMER3_BASE,
	kTimer4 = GPTM_TIMER4_BASE,
	kTimer5 = GPTM_TIMER5_BASE,
} Timer;

/*typedef enum {
	kWideTimer0 = GPTM_WTIMER0_BASE,
	kWideTimer1 = GPTM_WTIMER1_BASE,
	kWideTimer2 = GPTM_WTIMER2_BASE,
	kWideTimer3 = GPTM_WTIMER3_BASE,
	kWideTimer4 = GPTM_WTIMER4_BASE,
	kWideTimer5 = GPTM_WTIMER5_BASE,
} WideTimer;*/

static inline void timer_stop_solo(Timer timer) {
	uint32_t control_old = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, control_old & ~0x01);
}

static inline void timer_run_solo(Timer timer) {
	uint32_t control_old = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, control_old | 0x01);
}

static inline void timer_enable(Timer timer) {
	uint32_t rcgtimer_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCTIMER);
	switch (timer) {
		case kTimer0:
			rcgtimer_old |= 0x01;
			break;
		case kTimer1:
			rcgtimer_old |= 0x02;
			break;
		case kTimer2:
			rcgtimer_old |= 0x04;
			break;
		case kTimer3:
			rcgtimer_old |= 0x08;
			break;
		case kTimer4:
			rcgtimer_old |= 0x10;
			break;
		case kTimer5:
			rcgtimer_old |= 0x20;
			break;
	}
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCTIMER, rcgtimer_old);
}

static inline void timer_setup_oneshot_solo(Timer timer) {
	// clear GPTMCTL to completely disable the timer
	uint32_t old_control = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, old_control & ~ GPTM_TIMER_CONTROL_A_ENABLE);
	// set timer mode to the right mode in GPTMCFG
	uint32_t old_config = read_hw_u32((void *) timer, GPTM_TIMER_CONFIG);
	write_hw_u32((void *) timer, GPTM_TIMER_CONFIG, old_config & ~GPTM_TIMER_CONFIG_MASK);
	// set the timer to one-shot mode
	uint32_t old_mode = read_hw_u32((void *) timer, GPTM_TIMER_A_MODE);
	write_hw_u32((void *) timer, GPTM_TIMER_A_MODE, (old_mode & ~ GPTM_TIMER_MODE_MASK) | GPTM_TIMER_A_MODE_ONE_SHOT);
	
}

static inline void timer_setup_periodic_solo(Timer timer) {
	// clear GPTMCTL to completely disable the timer
	uint32_t old_control = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, old_control & ~ GPTM_TIMER_CONTROL_A_ENABLE);
	// set timer mode to the right mode in GPTMCFG
	uint32_t old_config = read_hw_u32((void *) timer, GPTM_TIMER_CONFIG);
	write_hw_u32((void *) timer, GPTM_TIMER_CONFIG, old_config & ~GPTM_TIMER_CONFIG_MASK);
	// set the timer to one-shot mode
	uint32_t old_mode = read_hw_u32((void *) timer, GPTM_TIMER_A_MODE);
	write_hw_u32((void *) timer, GPTM_TIMER_A_MODE, (old_mode & ~ GPTM_TIMER_MODE_MASK) | GPTM_TIMER_A_MODE_PERIODIC);
}

static inline void timer_set_interval_solo(Timer timer, uint32_t interval) {
	// write the interval load register
	write_hw_u32((void *) timer, GPTM_TIMER_A_INTERVAL, interval);
}

static inline bool timer_poll_timeout_solo(Timer timer) {
	// read the interrupt status of a timer
	return read_hw_u32((void *) timer, GPTM_TIMER_RAW_INTERRUPT_STATUS) & GPTM_TIMER_RAW_INTERRUPT_STATUS_A_TIMEOUT;
}

static inline void timer_reset_timeout_solo(Timer timer) {
	// clear the timeout bit in the timer interrupt status
	write_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_CLEAR, GPTM_TIMER_CLEAR_INTERRUPT_STATUS_A_TIMEOUT);
}

static inline void timer_disable_interrupt_solo(Timer timer) {
	// disable all interrupts
	write_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_MASK, 0);
}

static inline void timer_set_timeout_interrupt_solo(Timer timer, void (*handler)()) {
	switch(timer) {
		case kTimer0:
			set_isr(19, handler);
			break;
		case kTimer1:
			set_isr(21, handler);
			break;
		case kTimer2:
			set_isr(23, handler);
			break;
		case kTimer3:
			set_isr(35, handler);
			break;
		case kTimer4:
			set_isr(70, handler);
			break;
		case kTimer5:
			set_isr(92, handler);
			break;
		default:
			break;
	}
}

static inline void __timer_isr_set_a_enabled(Timer timer) {
	switch(timer) {
		case kTimer0:
			set_isr_enabled(19);
			break;
		case kTimer1:
			set_isr_enabled(21);
			break;
		case kTimer2:
			set_isr_enabled(23);
			break;
		case kTimer3:
			set_isr_enabled(35);
			break;
		case kTimer4:
			set_isr_enabled(70);
			break;
		case kTimer5:
			set_isr_enabled(92);
			break;
	}
}

static inline void timer_enable_timeout_interrupt_solo(Timer timer) {
	uint32_t old_mask = read_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_MASK);
	write_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_MASK, old_mask | GPTM_TIMER_INTERRUPT_MASK_A_TIMEOUT);
	__timer_isr_set_a_enabled(timer);
}

static inline void timer_disable_timeout_interrupt_solo(Timer timer) {
	uint32_t old_mask = read_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_MASK);
	write_hw_u32((void *) timer, GPTM_TIMER_INTERRUPT_MASK, old_mask & ~ GPTM_TIMER_INTERRUPT_MASK_A_TIMEOUT);
	__timer_isr_set_a_enabled(timer);
}

static inline void timer_enable_adc_trigger_solo(Timer timer) {
	uint32_t old_ctl = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, old_ctl | GPTM_TIMER_CONTROL_A_ADC);
}

static inline void timer_disable_adc_trigger_solo(Timer timer) {
	uint32_t old_ctl = read_hw_u32((void *) timer, GPTM_TIMER_CONTROL);
	write_hw_u32((void *) timer, GPTM_TIMER_CONTROL, old_ctl & ~ GPTM_TIMER_CONTROL_A_ADC);
}

static inline uint32_t timer_read_solo(Timer timer) {
	return read_hw_u32((void *) timer, GPTM_TIMER_A_VALUE);
}

#endif /* TIMER_H */
