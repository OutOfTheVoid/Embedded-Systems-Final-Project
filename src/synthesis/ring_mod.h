#ifndef RING_MOD_H
#define RING_MOD_H

#include "synthesis/sample_source.h"

typedef struct {
	SampleSource output;
	SampleSource * in_a;
	SampleSource * in_b;
} RingMod;

static float ring_mod_compute(float t, float dt, RingMod * ring_mod);

static inline void ring_mod_init(RingMod * ring_mod, SampleSource * in_a, SampleSource * in_b) {
	ring_mod -> in_a = in_a;
	ring_mod -> in_b = in_b;
	sample_source_init(& ring_mod -> output, (sample_source_compute_fn_t) ring_mod_compute, ring_mod);
}

static inline float ring_mod_compute(float t, float dt, RingMod * ring_mod) {
	float a = sample_source_get(ring_mod -> in_a, t, dt);
	if (a == 0.0f)
		return 0.0f;
	return a * sample_source_get(ring_mod -> in_b, t, dt);
}

#endif
