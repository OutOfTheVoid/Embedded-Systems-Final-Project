#ifndef CHANNEL_MIXER_H
#define CHANNEL_MIXER_H

#include <stdint.h>

#include "sample_source.h"

typedef struct {
	SampleSource output;
	SampleSource ** sources;
	uint8_t source_count;
} ChannelMixer;

static float channel_mixer_compute(float t, float dt, ChannelMixer * mixer);

void channel_mixer_init(ChannelMixer * mixer, SampleSource ** sources, uint8_t source_count) {
	mixer -> sources = sources;
	mixer -> source_count = source_count;
	sample_source_init(& mixer -> output, (sample_source_compute_fn_t )& channel_mixer_compute, mixer);
}

static inline float channel_mixer_compute(float t, float dt, ChannelMixer * mixer) {
	float tot = 0.0f;
	for (uint8_t i = 0; i < mixer ->source_count; i ++) {
		tot += sample_source_get(mixer -> sources[i], t, dt);
	}
	return tot / (float) mixer -> source_count;
}

#endif
