#ifndef ADSR_ENVELOPE_H
#define ADSR_ENVELOPE_H

#include "synthesis/sample_source.h"

#define __ADSR_STATE_ZERO 0
#define __ADSR_STATE_ATTACK 1
#define __ADSR_STATE_DECAY 2
#define __ADSR_STATE_SUSTAIN 3
#define __ADSR_STATE_RELEASE 4

typedef struct {
	SampleSource output;
	float period_time;
	float attack;
	float decay;
	float sustain;
	float release;
	int state;
	bool pending_release;
} ADSREnvelope;

static float adsr_envelope_compute(float t, float dt, ADSREnvelope * envelope);

static inline void adsr_envelope_init(ADSREnvelope * envelope, float attack, float decay, float sustain, float release) {
	envelope -> attack = attack;
	envelope -> decay = decay;
	envelope -> sustain = sustain;
	envelope -> release = release;
	envelope -> state = __ADSR_STATE_ZERO;
	envelope -> period_time = 0.0f;
	envelope -> pending_release = false;
	sample_source_init(& envelope -> output, (sample_source_compute_fn_t) & adsr_envelope_compute, envelope);
}

static inline void adsr_envelope_begin(ADSREnvelope * envelope) {
	envelope -> state = __ADSR_STATE_ATTACK;
	envelope -> period_time = 0.0f;
	envelope -> pending_release = false;
}

static inline void adsr_envelope_end(ADSREnvelope * envelope) {
	envelope -> pending_release = true;
}

static inline float adsr_envelope_compute(float t, float dt, ADSREnvelope * envelope) {
	switch (envelope -> state) {
		case __ADSR_STATE_ATTACK: {
			if (envelope -> period_time >= envelope -> attack) {
				envelope -> state = __ADSR_STATE_DECAY;
				envelope -> period_time = 0.0f;
				return 1.0f;
			}
			float value = envelope -> period_time / envelope -> attack;
			envelope -> period_time += dt;
			return value;
		}
		
		case __ADSR_STATE_DECAY: {
			if (envelope -> period_time >= envelope -> decay) {
				envelope -> state = __ADSR_STATE_SUSTAIN;
				return envelope -> sustain;
			}
			float value = (1.0f - (envelope -> period_time / envelope -> decay)) * (1.0f - envelope -> sustain) + envelope -> sustain;
			envelope -> period_time += dt;
			return value;
		}
		
		case __ADSR_STATE_SUSTAIN:
			if (envelope -> pending_release) {
				envelope -> state = __ADSR_STATE_RELEASE;
				envelope -> period_time = 0.0f;
			}
			return envelope -> sustain;
			
		case __ADSR_STATE_RELEASE: {
			if (envelope -> period_time >= envelope -> release) {
				envelope -> state = __ADSR_STATE_ZERO;
				return 0.0f;
			}
			float value = (1.0f - (envelope -> period_time / envelope -> release)) * envelope -> sustain;
			envelope -> period_time += dt;
			return value;
		}
		
		default:
		case __ADSR_STATE_ZERO:
			break;
	}
	
	return 0.0f;
}

#endif
