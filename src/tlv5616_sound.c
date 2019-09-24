#include "tlv5616_sound.h"

#include <stdint.h>

#include "isr.h"
#include "gpio.h"
#include "ssi.h"
#include "timer.h"
#include "clock.h"

#define TLV5616_CONTROL_FASTMODE 0x4000

void tlv5616_sound_interrupt();

uint16_t tlv5616_sound_audio_frame_buffers[TLV5616_AUDIO_FRAME_SIZE * 2];
uint32_t tlv5616_sound_audio_read_offset = 0;
bool tlv5616_sound_audio_frame_read_is_zero;
bool tlv5616_sound_audio_frame_write_filled;

void tlv5616_sound_init() {
	for (int i = 0; i < TLV5616_AUDIO_FRAME_SIZE * 2; i ++) {
		tlv5616_sound_audio_frame_buffers[i] = 0;
	}
	tlv5616_sound_audio_frame_read_is_zero = true;
	tlv5616_sound_audio_frame_write_filled = false;
	
	gpio_enable_port(kPortD);
	ssi_enable(kSSI1_PortD);
	ssi_setup(kSSI1_PortD, kSSIPinType_Clock | kSSIPinType_TX | kSSIPinType_FrameSignal, kSSIFrameFormat_FreescaleSPI_FirstEdge, true, true, 0, 2, 16, true, kDriveCurrent_2ma, kPinDrive_PullDown);
	
	timer_enable(kTimer5);
	timer_setup_periodic_solo(kTimer5);
	timer_set_interval_solo(kTimer5, get_clock_speed_hz() / TLV5616_SOUND_RATE);
	timer_set_timeout_interrupt_solo(kTimer5, tlv5616_sound_interrupt);
	timer_reset_timeout_solo(kTimer5);
	timer_enable_timeout_interrupt_solo(kTimer5);
}

void tlv5616_sound_start() {
	ssi_start(kSSI1_PortD);
	timer_run_solo(kTimer5);
}

bool tlv5616_sound_needs_audio_frame() {
	uint32_t int_status = block_interrupts();
	bool needs = ! tlv5616_sound_audio_frame_write_filled;
	restore_interrupts(int_status);
	return needs;
}

uint16_t * tlv5616_sound_get_framebuffer() {
	uint32_t int_status = block_interrupts();
	uint16_t * new_frame_buff = & tlv5616_sound_audio_frame_buffers[tlv5616_sound_audio_frame_read_is_zero ? TLV5616_AUDIO_FRAME_SIZE : 0];
	restore_interrupts(int_status);
	return new_frame_buff;
}

void tlv5616_sound_submit_framebuffer() {
	uint32_t int_status = block_interrupts();
	tlv5616_sound_audio_frame_write_filled = true;
	restore_interrupts(int_status);
}

void __attribute__((interrupt)) tlv5616_sound_interrupt () {
	timer_set_interval_solo(kTimer5, get_clock_speed_hz() / TLV5616_SOUND_RATE);
	timer_reset_timeout_solo(kTimer5);
	uint32_t int_status = block_interrupts();
	uint16_t sample = tlv5616_sound_audio_frame_buffers[(tlv5616_sound_audio_frame_read_is_zero ? 0 : TLV5616_AUDIO_FRAME_SIZE) + tlv5616_sound_audio_read_offset];
	tlv5616_sound_audio_read_offset ++;
	tlv5616_sound_audio_read_offset %= TLV5616_AUDIO_FRAME_SIZE;
	if ((tlv5616_sound_audio_read_offset == 0) && tlv5616_sound_audio_frame_write_filled) {
		tlv5616_sound_audio_frame_write_filled = false;
		tlv5616_sound_audio_frame_read_is_zero = ! tlv5616_sound_audio_frame_read_is_zero;
	}
	uint16_t packet = (sample >> 5) | TLV5616_CONTROL_FASTMODE;
	ssi_transmit(kSSI1_PortD, packet);
	restore_interrupts(int_status);
}
