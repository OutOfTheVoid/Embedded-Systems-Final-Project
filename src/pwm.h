#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"
#include "gpio.h"

typedef enum {
	kPWM0 = 0,
	kPWM1 = 1
} PWM;

typedef enum {
	kPWMClockDivisor_1 = 0xFF,
	kPWMClockDivisor_2 = 0x0,
	kPWMClockDivisor_4 = 0x1,
	kPWMClockDivisor_8 = 0x2,
	kPWMClockDivisor_16 = 0x3,
	kPWMClockDivisor_32 = 0x4,
	kPWMClockDivisor_64 = 0x5,
} PWMClockDivisor;

typedef enum {
	kPWMGen0 = 0,
	kPWMGen1 = 1,
	kPWMGen2 = 2,
	kPWMGen3 = 3
} PWMGen;

typedef enum {
	kPWMPin_PWM0_Fault_PF2 = 0,
	kPWMPin_PWM0_Fault_PD6 = 1,
	kPWMPin_PWM0_Fault_PD2 = 2,
	kPWMPin_PWM0_0_PB6 = 3,
	kPWMPin_PWM0_1_PB7 = 4,
	kPWMPin_PWM0_2_PB4 = 5,
	kPWMPin_PWM0_3_PB5 = 6,
	kPWMPin_PWM0_4_PE4 = 7,
	kPWMPin_PWM0_5_PE5 = 8,
	kPWMPin_PWM0_6_PC4 = 9,
	kPWMPin_PWM0_6_PD0 = 10,
	kPWMPin_PWM0_7_PC5 = 11,
	kPWMPin_PWM0_7_PD1 = 12,
	
	kPWMPin_PWM1_Fault_PF4 = 13,
	kPWMPin_PWM1_0_PD0 = 14,
	kPWMPin_PWM1_1_PD1 = 15,
	kPWMPin_PWM1_2_PA6 = 16,
	kPWMPin_PWM1_2_PE4 = 17,
	kPWMPin_PWM1_3_PA7 = 18,
	kPWMPin_PWM1_3_PE5 = 19,
	kPWMPin_PWM1_4_PF0 = 20,
	kPWMPin_PWM1_5_PF1 = 21,
	kPWMPin_PWM1_6_PF2 = 22,
	kPWMPin_PWM1_7_PF3 = 23,
} PWMPin;

typedef enum {
	kPWMGenMode_LoadHighCompareLowA = 0,
	//kPWMGenMode_Deadband_
} PWMGenMode;

typedef struct {
	void * base;
	uint8_t pin_function;
} PWMInfo;

typedef struct {
	uintptr_t ctl_offset;
	uintptr_t gen_a_ctl_offset;
	uintptr_t gen_b_ctl_offset;
	uintptr_t load_offset;
	uintptr_t cmp_a_offset;
	uintptr_t cmp_b_offset;
} PWMGenInfo;

typedef struct {
	PWM pwm;
	uint8_t pin;
	Port port;
	bool output;
	uint8_t pwm_output_num;
	uint8_t function;
} PWMPinInfo;

static PWMInfo __pwm_info[] = {
	[kPWM0] = {.base = (void *) PWM_PWM0_BASE, .pin_function = 4},
	[kPWM1] = {.base = (void *) PWM_PWM1_BASE, .pin_function = 5}
};

static PWMGenInfo __pwm_gen_info[] = {
	[kPWMGen0] = {.ctl_offset = PWM_0CTL, .gen_a_ctl_offset = PWM_0GENA, .gen_b_ctl_offset = PWM_0GENB, .load_offset = PWM_0LOAD, .cmp_a_offset = PWM_0CMPA, .cmp_b_offset = PWM_0CMPB},
	[kPWMGen1] = {.ctl_offset = PWM_1CTL, .gen_a_ctl_offset = PWM_1GENA, .gen_b_ctl_offset = PWM_1GENB, .load_offset = PWM_1LOAD, .cmp_a_offset = PWM_1CMPA, .cmp_b_offset = PWM_1CMPB},
	[kPWMGen2] = {.ctl_offset = PWM_2CTL, .gen_a_ctl_offset = PWM_2GENA, .gen_b_ctl_offset = PWM_2GENB, .load_offset = PWM_2LOAD, .cmp_a_offset = PWM_2CMPA, .cmp_b_offset = PWM_2CMPB},
	[kPWMGen3] = {.ctl_offset = PWM_3CTL, .gen_a_ctl_offset = PWM_3GENA, .gen_b_ctl_offset = PWM_3GENB, .load_offset = PWM_3LOAD, .cmp_a_offset = PWM_3CMPA, .cmp_b_offset = PWM_3CMPB},
};

static PWMPinInfo __pwm_pin_info[] = {
	[kPWMPin_PWM0_Fault_PF2] = {.pwm = kPWM0, .port = kPortF, .pin = 2, .output = false},
	[kPWMPin_PWM0_Fault_PD6] = {.pwm = kPWM0, .port = kPortD, .pin = 6, .output = false},
	[kPWMPin_PWM0_Fault_PD2] = {.pwm = kPWM0, .port = kPortD, .pin = 2, .output = false},
	[kPWMPin_PWM0_0_PB6] = {.pwm = kPWM0, .port = kPortB, .pin = 6, .output = true, .pwm_output_num = 0},
	[kPWMPin_PWM0_1_PB7] = {.pwm = kPWM0, .port = kPortB, .pin = 7, .output = true, .pwm_output_num = 1},
	[kPWMPin_PWM0_2_PB4] = {.pwm = kPWM0, .port = kPortB, .pin = 4, .output = true, .pwm_output_num = 2},
	[kPWMPin_PWM0_3_PB5] = {.pwm = kPWM0, .port = kPortB, .pin = 5, .output = true, .pwm_output_num = 3},
	[kPWMPin_PWM0_4_PE4] = {.pwm = kPWM0, .port = kPortE, .pin = 4, .output = true, .pwm_output_num = 4},
	[kPWMPin_PWM0_5_PE5] = {.pwm = kPWM0, .port = kPortE, .pin = 5, .output = true, .pwm_output_num = 5},
	[kPWMPin_PWM0_6_PC4] = {.pwm = kPWM0, .port = kPortC, .pin = 4, .output = true, .pwm_output_num = 6},
	[kPWMPin_PWM0_6_PD0] = {.pwm = kPWM0, .port = kPortD, .pin = 0, .output = true, .pwm_output_num = 6},
	[kPWMPin_PWM0_7_PC5] = {.pwm = kPWM0, .port = kPortC, .pin = 5, .output = true, .pwm_output_num = 7},
	[kPWMPin_PWM0_7_PD1] = {.pwm = kPWM0, .port = kPortD, .pin = 1, .output = true, .pwm_output_num = 7},
	[kPWMPin_PWM1_0_PD0] = {.pwm = kPWM1, .port = kPortD, .pin = 0, .output = true, .pwm_output_num = 0},
	[kPWMPin_PWM1_1_PD1] = {.pwm = kPWM1, .port = kPortD, .pin = 1, .output = true, .pwm_output_num = 1},
	[kPWMPin_PWM1_2_PA6] = {.pwm = kPWM1, .port = kPortA, .pin = 6, .output = true, .pwm_output_num = 2},
	[kPWMPin_PWM1_2_PE4] = {.pwm = kPWM1, .port = kPortE, .pin = 4, .output = true, .pwm_output_num = 2},
	[kPWMPin_PWM1_3_PA7] = {.pwm = kPWM1, .port = kPortA, .pin = 7, .output = true, .pwm_output_num = 3},
	[kPWMPin_PWM1_3_PE5] = {.pwm = kPWM1, .port = kPortE, .pin = 5, .output = true, .pwm_output_num = 3},
	[kPWMPin_PWM1_4_PF0] = {.pwm = kPWM1, .port = kPortF, .pin = 0, .output = true, .pwm_output_num = 4},
	[kPWMPin_PWM1_5_PF1] = {.pwm = kPWM1, .port = kPortF, .pin = 1, .output = true, .pwm_output_num = 5},
	[kPWMPin_PWM1_6_PF2] = {.pwm = kPWM1, .port = kPortF, .pin = 2, .output = true, .pwm_output_num = 6},
	[kPWMPin_PWM1_7_PF3] = {.pwm = kPWM1, .port = kPortF, .pin = 3, .output = true, .pwm_output_num = 7},
	[kPWMPin_PWM1_Fault_PF4] = {.pwm = kPWM1, .port = kPortF, .pin = 4, .output = false},
};

static inline void pwm_enable(PWM pwm) {
	uint32_t rcgc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCPWM);
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCPWM, rcgc_old | (1 << pwm));
}

static inline void pwm_disable(PWM pwm) {
	uint32_t rcgc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCPWM);
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCPWM, rcgc_old & ~ (1 << pwm));
}

static inline void pwm_pins_setup(PWMPin pins[], uint32_t pin_count, DriveCurrent current, PinDrive drive) {
	for (int i = 0; i < pin_count; i ++) {
		gpio_enable_port(__pwm_pin_info[pins[i]].port);
		gpio_disable_digital_pin(__pwm_pin_info[pins[i]].port, __pwm_pin_info[pins[i]].pin);
		gpio_setup_digital_pin_group_alternate_function(__pwm_pin_info[pins[i]].port, 1 << __pwm_pin_info[pins[i]].pin, __pwm_info[__pwm_pin_info[pins[i]].pwm].pin_function, __pwm_pin_info[pins[i]].output, current, drive);
	}
}

static inline void pwm_pins_enable(PWM pwm, PWMPin pins[], uint32_t pin_count) {
	uint32_t enable_old = read_hw_u32(__pwm_info[pwm].base, PWM_ENABLE);
	for (int i = 0; i < pin_count; i ++) {
		gpio_enable_port(__pwm_pin_info[pins[i]].port);
		gpio_enable_digital_pin(__pwm_pin_info[pins[i]].port, __pwm_pin_info[pins[i]].pin);
		enable_old |= 1 << __pwm_pin_info[pins[i]].pwm_output_num;
	}
	write_hw_u32(__pwm_info[pwm].base, PWM_ENABLE, enable_old);
}

static inline void pwm_pins_disable(PWM pwm, PWMPin pins[], uint32_t pin_count) {
	uint32_t enable_old = read_hw_u32(__pwm_info[pwm].base, PWM_ENABLE);
	for (int i = 0; i < pin_count; i ++) {
		gpio_disable_digital_pin(__pwm_pin_info[pins[i]].port, __pwm_pin_info[pins[i]].pin);
		enable_old &= ~(1 << __pwm_pin_info[pins[i]].pwm_output_num);
	}
	write_hw_u32(__pwm_info[pwm].base, PWM_ENABLE, enable_old);
}

static inline void pwm_global_clock_divisor_set(PWMClockDivisor clock_divisor) {
	uint32_t rcc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC);
	if (clock_divisor == kPWMClockDivisor_1) {
		write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC, rcc_old & ~ SYSCTL_RCC_USEPWMCLOCKDIV);
	} else {
		write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCC, ((rcc_old | SYSCTL_RCC_USEPWMCLOCKDIV) & ~ SYSCTL_RCC_PWMCLOCKDIV_MASK) | (((uint32_t) clock_divisor) << SYSCTL_RCC_PWMCLOCKDIV_SHIFT));
	}
}

static inline uint32_t __pwm_get_gen_ctl_bits(PWMGenMode mode) {
	switch (mode) {
		case kPWMGenMode_LoadHighCompareLowA: {
			return ((PWM_GEN_EVT_HIGH) << PWM_GEN_LOAD_SHIFT) | 
			       ((PWM_GEN_EVT_LOW) << PWM_GEN_COMP_A_DOWN_SHIFT);
		}
		break;
		
		default:
		break;
	}
	return 0;
}

static inline void pwm_generator_setup(PWM pwm, PWMGen pwm_gen, PWMGenMode gen_a_mode, PWMGenMode gen_b_mode) {
	uint32_t ctl_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset, ctl_old & ~ PWM_NCTL_MASK);
	uint32_t gen_a_ctl_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].gen_a_ctl_offset);
	uint32_t gen_b_ctl_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].gen_b_ctl_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].gen_a_ctl_offset, (gen_a_ctl_old & ~ PWM_GEN_MASK) | __pwm_get_gen_ctl_bits(gen_a_mode));
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].gen_b_ctl_offset, (gen_b_ctl_old & ~ PWM_GEN_MASK) | __pwm_get_gen_ctl_bits(gen_b_mode));
}

static inline void pwm_generator_set_reload(PWM pwm, PWMGen pwm_gen, uint16_t reload) {
	uint32_t load_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].load_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].load_offset, (load_old & 0xFFFF0000) | (uint32_t) reload);
}

static inline void pwm_generator_set_compare_a(PWM pwm, PWMGen pwm_gen, uint16_t compare) {
	uint32_t compare_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].cmp_a_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].cmp_a_offset, (compare_old & 0xFFFF0000) | (uint32_t) compare);
}

static inline void pwm_generator_set_compare_b(PWM pwm, PWMGen pwm_gen, uint16_t compare) {
	uint32_t compare_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].cmp_b_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].cmp_b_offset, (compare_old & 0xFFFF0000) | (uint32_t) compare);
}

static inline void pwm_generator_enable(PWM pwm, PWMGen pwm_gen) {
	uint32_t ctl_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset, ctl_old | PWM_CTL_ENABLE);
}

static inline void pwm_generator_disable(PWM pwm, PWMGen pwm_gen) {
	uint32_t ctl_old = read_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset);
	write_hw_u32(__pwm_info[pwm].base, __pwm_gen_info[pwm_gen].ctl_offset, ctl_old & ~PWM_CTL_ENABLE);
}

#endif
