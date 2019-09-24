#ifndef SYNTH_VOICE_H
#define SYNTH_VOICE_H

#include "synthesis/sample_source.h"
#include "synthesis/constant_source.h"
#include "synthesis/oscillator.h"
#include "synthesis/ring_mod.h"
#include "synthesis/adsr_envelope.h"

typedef struct {
	SampleSource output;
	bool active;
	float last_deactivation_t;
	int key_code;
	Oscillator oscillator_1;
	ConstantSource note_freq_src;
	ADSREnvelope envelope;
	RingMod envelope_mod;
} SynthVoice;

static float synth_voice_compute(float t, float dt, SynthVoice * voice);

static inline void synth_voice_init(SynthVoice * voice) {
	voice -> last_deactivation_t = - 1.0f;
	constant_source_init(& voice -> note_freq_src, 1.0f);
	oscillator_init(& voice -> oscillator_1, kOscillatorType_Triangle, & voice -> note_freq_src.output);
	
	adsr_envelope_init(& voice -> envelope, 0.001f, 0.1f, 0.5f, 0.2f);
	ring_mod_init(& voice -> envelope_mod, & voice -> envelope.output, & voice -> oscillator_1.output);
	
	sample_source_init(& voice -> output, (sample_source_compute_fn_t) synth_voice_compute, voice);
}

static inline void synth_voice_set_osc1_type(SynthVoice * voice, OscillatorType type) {
	oscillator_init(& voice -> oscillator_1, type, & voice -> note_freq_src.output);
}

static inline void synth_voice_begin_note(SynthVoice * voice, float frequency) {
	voice -> active = true;
	voice -> note_freq_src.constant = frequency;
	adsr_envelope_begin(& voice -> envelope);
}

static inline void synth_voice_end_note(SynthVoice * voice) {
	voice -> active = false;
	adsr_envelope_end(& voice -> envelope);
}

static inline float synth_voice_compute(float t, float dt, SynthVoice * voice) {
	if (voice -> active) {
		voice -> last_deactivation_t = t;
	}
	return sample_source_get(& voice -> envelope_mod.output, t, dt);
}

#endif
