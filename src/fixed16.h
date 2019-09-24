#ifndef FIXED16_H
#define FIXED16_H

#include <stdint.h>
#include <stdbool.h>

typedef int16_t fixed16_t;

#define FIXED16_ZERO 0
#define FIXED16_ONE 0x100
#define FIXED16_NEGATIVE_ONE ((- 0x100))
#define FIXED16_PI 0x0324
#define FIXED16_TAU 0x0648
#define FIXED16_E 0x02B7

#define FIXED16_MAX_PRINT 12
// -128.000001

inline static fixed16_t float_to_fixed16(float x) {
	return (fixed16_t) (x * 256.0f);
}

inline static fixed16_t i8_to_fixed16(int8_t x) {
	return (fixed16_t) (x * 256);
}

inline static fixed16_t add_fx16(fixed16_t lhs, fixed16_t rhs) {
	return lhs + rhs;
}

inline static fixed16_t sub_fx16(fixed16_t lhs, fixed16_t rhs) {
	return lhs - rhs;
}

inline static fixed16_t mul_fx16(fixed16_t lhs, fixed16_t rhs) {
	return (fixed16_t) (((int32_t) lhs * (int32_t) rhs) / 0x100);
}

inline static fixed16_t div_fx16(fixed16_t numerator, fixed16_t denominator) {
	int64_t num = (int64_t) numerator;
	int64_t den = (int64_t) denominator;
	num *= 0x100;
	return (int16_t) (num / den);
}

inline static int8_t whole(fixed16_t x) {
	return (int8_t) (x / 0x100);
}

inline static int16_t fraction_thousandths(fixed16_t x) {
	bool negative = x < 0;
	int32_t thousandths_positive = (negative ? - x : x) & 0xFF; // fractional, non-sign bits
	thousandths_positive *= 1000;
	thousandths_positive /= 256;
	return (int16_t) (negative ? - thousandths_positive : thousandths_positive);
}

inline static int32_t fraction_millionths(fixed16_t x) {
	bool negative = x < 0;
	int32_t millionths_positive = (negative ? - x : x) & 0xFF; // fractional, non-sign bits
	millionths_positive *= 1000000;
	millionths_positive /= 256;
	return negative ? - millionths_positive : millionths_positive;
}

inline static int fixed16_to_string(fixed16_t x, char * buff) {
	uint32_t fract_millionths = fraction_millionths(x);
	uint32_t integer = whole(x);
	int i = 0;
	
	if (x < 0) {
		buff[i] = '-';
		i ++;
		integer = - integer;
		fract_millionths = - fract_millionths;
	}
	
	if (integer >= 100) {
		buff[i] = "0123456789"[integer / 100];
		i ++;
	}
	if (integer >= 10) {
		buff[i] = "0123456789"[(integer / 10) % 10];
		i ++;
	}
	buff[i] = "0123456789"[integer % 10];
	i ++;
	
	if (fract_millionths == 0) {
		buff[i] = 0;
		return i;
	}
	
	buff[i] = '.';
	i ++;
	for (int d = 100000; d > 0; d /= 10) {
		if (fract_millionths == 0) {
			buff[i] = 0;
			return i;
		}
		uint32_t digit = fract_millionths / d;
		buff[i] = "0123456789"[digit];
		fract_millionths -= digit * d;
		i ++;
	}
	buff[i] = 0;
	return i;
}

fixed16_t fx16_sign_lookup[] = {
	0,
	6,
	12,
	18,
	25,
	31,
	37,
	43,
	49,
	56,
	62,
	68,
	74,
	80,
	86,
	92,
	97,
	103,
	109,
	115,
	120,
	126,
	131,
	136,
	142,
	147,
	152,
	157,
	162,
	167,
	171,
	176,
	181,
	185,
	189,
	193,
	197,
	201,
	205,
	209,
	212,
	216,
	219,
	222,
	225,
	228,
	231,
	234,
	236,
	238,
	241,
	243,
	244,
	246,
	248,
	249,
	251,
	252,
	253,
	254,
	254,
	255,
	255,
	255,
};

fixed16_t sin_fx16(fixed16_t t) {
	int lut_idx = div_fx16(t, FIXED16_PI * 2) % 256;
	if (lut_idx < 0)
		lut_idx += 256;
	int lut_quad = lut_idx / 64;
	lut_idx %= 64;
	switch (lut_quad)
	{
		case 0:
			return fx16_sign_lookup[lut_idx];
		case 1:
			return fx16_sign_lookup[63 - lut_idx];
		case 2:
			return -fx16_sign_lookup[lut_idx];
		case 3:
			return - fx16_sign_lookup[63 - lut_idx];
	}
	return 0;
}

fixed16_t cos_fx16(fixed16_t t) {
	return sin_fx16(t + FIXED16_PI / 2);
}

#endif
