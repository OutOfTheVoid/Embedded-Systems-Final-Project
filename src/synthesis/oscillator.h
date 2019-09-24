#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "sample_source.h"
#include "math.h"

#define OSC_MAX_PHASES 5

#define TAU 6.28318530718f

float __oscillator_supersaw_phase_table[OSC_MAX_PHASES] = {
	1.0f,
	1.0008362909f,
	0.9994682999f,
	0.9997777767f,
	1.0067839420f,
};

typedef enum {
	kOscillatorType_Sine = 0,
	kOscillatorType_Triangle = 1,
	kOScillatorType_Sawtooth = 2,
	kOscillatorType_Square = 3,
	kOscillatorType_Supersaw = 4,
} OscillatorType;

typedef struct {
	SampleSource output;
	SampleSource * freq_src;
	OscillatorType type;
	float phase[OSC_MAX_PHASES];
} Oscillator;

static float oscillator_compute(float t, float dt, Oscillator * oscillator);

static inline void oscillator_init(Oscillator * oscillator, OscillatorType type, SampleSource * frequency_source) {
	oscillator -> type = type;
	for (int i = 0; i < OSC_MAX_PHASES; i ++) {
		oscillator -> phase[i] = 0.0f;
	}
	oscillator -> freq_src = frequency_source;
	sample_source_init(& oscillator -> output, (sample_source_compute_fn_t) & oscillator_compute, oscillator);
}

static inline float oscillator_compute(float t, float dt, Oscillator * oscillator) {
	float freq = sample_source_get(oscillator -> freq_src, t, dt);
	float d_phase = dt * freq;
	switch (oscillator -> type) {
		case kOscillatorType_Sine: {
			oscillator -> phase[0] += d_phase;
			oscillator -> phase[0] = fmod(oscillator -> phase[0], 1.0f);
			return sinf(oscillator -> phase[0] * TAU);
		}
			
		case kOscillatorType_Triangle: {
			oscillator -> phase[0] += d_phase;
			oscillator -> phase[0] = fmod(oscillator -> phase[0], 1.0f);
			float x = oscillator -> phase[0] * 2.0f;
			if (x > 1.0f)
				x = 2.0f - x;
			return 2.0f * x - 1.0f;
		}
		
		case kOScillatorType_Sawtooth: {
			oscillator -> phase[0] += d_phase;
			oscillator -> phase[0] = fmod(oscillator -> phase[0], 1.0f);
			return oscillator -> phase[0] * 2.0f - 1.0f;
		}
		
		case kOscillatorType_Square: {
			oscillator -> phase[0] += d_phase;
			oscillator -> phase[0] = fmod(oscillator -> phase[0], 1.0f);
			return oscillator -> phase[0] < 0.5f ? - 1.0f : 1.0f;
		}
		
		case kOscillatorType_Supersaw: {
			float tot = 0.0f;
			for (int i = 0; i < OSC_MAX_PHASES; i ++) {
				oscillator -> phase[i] += d_phase * __oscillator_supersaw_phase_table[i];
				oscillator -> phase[i] = fmod(oscillator -> phase[i], 1.0f);
				tot += oscillator -> phase[i] * 2.0f - 1.0f;
			}
			return tot / (float) OSC_MAX_PHASES;
		}
		
		default:
			break;
	}
	return 0.0f;
}

#endif
