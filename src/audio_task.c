#include "audio_task.h"

#include "FreeRTOS.h"
#include "freertos_support.h"
#include "task.h"
#include "semphr.h"

#include "keyboard_task.h"
#include "tlv5616_sound.h"

#include "synthesis/sample_source.h"
#include "synthesis/synth_global.h"

#include "note_frequencies.h"

#include <math.h>

void gen_audio();
void audio_task();

#define AUDIO_STACK_DEPTH 100
#define AUDIO_TASK_PRIORITY 20

uint32_t audio_tick;

bool c_key_state;
bool g_key_state;
SynthVoice * c_key_voice;
SynthVoice * g_key_voice;

#define C_KEYCODE 1
#define G_KEYCODE 2

bool audio_task_init() {
	audio_tick = 0;
	
	tlv5616_sound_init();
	
	c_key_state = false;
	g_key_state = false;
	
	gpio_enable_port(kPortF);
	gpio_setup_digital_input_pin(kPortF, 0, kPinDrive_PullUp);
	gpio_setup_digital_input_pin(kPortF, 4, kPinDrive_PullUp);
	gpio_enable_digital_pin(kPortF, 0);
	gpio_enable_digital_pin(kPortF, 4);
	
	voice_type = kOscillatorType_Triangle;
	
	if (xTaskCreate(& audio_task, "audio_task", AUDIO_STACK_DEPTH, NULL, tskIDLE_PRIORITY + AUDIO_TASK_PRIORITY, NULL) != pdTRUE) {
		return false;
	}
	return true;
}

volatile uint32_t voice_type;

typedef struct {
	uint16_t key_bit;
	bool state;
	bool new_state;
	SynthVoice * voice;
	int freq;
} KeyData;

KeyData key_data[] = {
	{.key_bit = KEY_C, .state = false, .voice = NULL, .freq = NOTE_C_3},
	{.key_bit = KEY_Cs, .state = false, .voice = NULL, .freq = NOTE_Cs_3},
	{.key_bit = KEY_D, .state = false, .voice = NULL, .freq = NOTE_D_3},
	{.key_bit = KEY_Ds, .state = false, .voice = NULL, .freq = NOTE_Ds_3},
	{.key_bit = KEY_E, .state = false, .voice = NULL, .freq = NOTE_E_3},
	{.key_bit = KEY_F, .state = false, .voice = NULL, .freq = NOTE_F_3},
	{.key_bit = KEY_Fs, .state = false, .voice = NULL, .freq = NOTE_Fs_3},
	{.key_bit = KEY_G, .state = false, .voice = NULL, .freq = NOTE_G_3},
	{.key_bit = KEY_Gs, .state = false, .voice = NULL, .freq = NOTE_Gs_3},
	{.key_bit = KEY_A, .state = false, .voice = NULL, .freq = NOTE_A_3},
	{.key_bit = KEY_As, .state = false, .voice = NULL, .freq = NOTE_As_3},
	{.key_bit = KEY_B, .state = false, .voice = NULL, .freq = NOTE_B_3},
};

void audio_task(void * unused) {
	(void) unused;
	keybaord_keys_t * keys;
	synth_global_init();
	tlv5616_sound_start();
	while (true) {
		uint32_t int_state = block_interrupts();
		uint32_t voice_type_new = voice_type;
		restore_interrupts(int_state);
		
		keys = keyboard_get_next_keymap();
		for (int i = 0; i < 12; i ++) {
			key_data[i].new_state = (keys -> key_map & key_data[i].key_bit);
			if (key_data[i].new_state && ! key_data[i].state) {
				key_data[i].state = true;
				key_data[i].voice = synth_global_allocate_next_voice(key_data[i].freq);
				key_data[i].voice -> key_code = key_data[i].freq;
				synth_voice_set_osc1_type(key_data[i].voice, voice_type_new);
				synth_voice_begin_note(key_data[i].voice, note_frequencies[key_data[i].freq] * 2.0f);
			}
			if (! key_data[i].new_state && key_data[i].state) {
				key_data[i].state = false;
				if (key_data[i].voice -> key_code == key_data[i].freq) {
					synth_voice_end_note(key_data[i].voice);
				}
			}
		}
		
		if (tlv5616_sound_needs_audio_frame()) {
			gen_audio(tlv5616_sound_get_framebuffer(), & keys);
			tlv5616_sound_submit_framebuffer();
		} else {
			taskYIELD();
		}
	}
}

void gen_audio(uint16_t * buffer, keybaord_keys_t * keys) {
	// TODO: generate audio
	for (int i = 0; i < TLV5616_AUDIO_FRAME_SIZE; i ++) {
		float t = ((float) audio_tick) / ((float) TLV5616_SOUND_RATE);
		buffer[i] = (uint16_t) (32767.5f + 32767.5f * sample_source_get(synth_global_sample_source, t, 1.0f / (float) TLV5616_SOUND_RATE));
		audio_tick ++;
	}
}
