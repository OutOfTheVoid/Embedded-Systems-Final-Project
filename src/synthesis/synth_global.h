#ifndef SYNTH_GLOBAL_H
#define SYNTH_GLOBAL_H

#include "synthesis/sample_source.h"
#include "synthesis/synth_voice.h"
#include "synthesis/channel_mixer.h"
#include "synthesis/constant_source.h"

#define SYNTH_VOICE_COUNT 3

SampleSource * synth_global_sample_source;
SynthVoice synth_global_voices[SYNTH_VOICE_COUNT];

ChannelMixer __synth_global_voice_mixer;
SampleSource * __synth_global_voice_sources[SYNTH_VOICE_COUNT];

static inline void synth_global_init() {
	for (int i = 0; i < SYNTH_VOICE_COUNT; i ++) {
		synth_voice_init(& synth_global_voices[i]);
		__synth_global_voice_sources[i] = & synth_global_voices[i].output;
	}
	channel_mixer_init(& __synth_global_voice_mixer, __synth_global_voice_sources, SYNTH_VOICE_COUNT);
	synth_global_sample_source = & __synth_global_voice_mixer.output;
}

static inline SynthVoice * synth_global_allocate_next_voice(int key_code) {
	float earilest_deactivation_t = synth_global_voices[0].last_deactivation_t;
	SynthVoice * earliest_deactivated_voice = & synth_global_voices[0];
	if (synth_global_voices[0].key_code == key_code) {
		return & synth_global_voices[0];
	}
	for (int i = 1; i < SYNTH_VOICE_COUNT; i ++) {
		if (synth_global_voices[i].key_code == key_code) {
			return & synth_global_voices[i];
		}
		if (synth_global_voices[i].last_deactivation_t < earilest_deactivation_t) {
			earliest_deactivated_voice = & synth_global_voices[i];
			earilest_deactivation_t = synth_global_voices[i].last_deactivation_t;
		}
	}
	return earliest_deactivated_voice;
}

#endif
