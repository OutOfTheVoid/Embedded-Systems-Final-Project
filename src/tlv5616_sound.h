#ifndef TLV5616_SOUND_H
#define TLV5616_SOUND_H

#define TLV5616_SOUND_RATE 10000
#define TLV5616_AUDIO_FRAME_SIZE 64

#include <stdbool.h>
#include <stdint.h>

void tlv5616_sound_init();
void tlv5616_sound_start();


bool tlv5616_sound_needs_audio_frame();
uint16_t * tlv5616_sound_get_framebuffer();
void tlv5616_sound_submit_framebuffer();

#endif
