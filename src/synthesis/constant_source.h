#ifndef CONSTANT_SOURCE_H
#define CONSTANT_SOURCE_H

#include "sample_source.h"

typedef struct {
	SampleSource output;
	float constant;
} ConstantSource;

static inline float constant_source_compute(float t, float dt, ConstantSource * source) {
	return source -> constant;
}

static inline void constant_source_init(ConstantSource * constant_source, float value) {
	constant_source -> constant = value;
	sample_source_init(& constant_source -> output, (sample_source_compute_fn_t) & constant_source_compute, constant_source); 
}

#endif
