#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "hw_rw.h"

typedef struct {
	void * source_end;
	void * destination_end;
	uint32_t control;
	uint32_t unused;
} __attribute__((packed)) DMAControlChannel;

typedef enum {
	kDMAChannel_0 = 0,
	kDMAChannel_1 = 1,
	kDMAChannel_2 = 2,
	kDMAChannel_3 = 3,
	kDMAChannel_4 = 4,
	kDMAChannel_5 = 5,
	kDMAChannel_6 = 6,
	kDMAChannel_7 = 7,
	kDMAChannel_8 = 8,
	kDMAChannel_9 = 9,
	kDMAChannel_10 = 10,
	kDMAChannel_11 = 11,
	kDMAChannel_12 = 12,
	kDMAChannel_13 = 13,
	kDMAChannel_14 = 14,
	kDMAChannel_15 = 15,
	kDMAChannel_16 = 16,
	kDMAChannel_17 = 17,
	kDMAChannel_18 = 18,
	kDMAChannel_19 = 19,
	kDMAChannel_20 = 20,
	kDMAChannel_21 = 21,
	kDMAChannel_22 = 22,
	kDMAChannel_23 = 23,
	kDMAChannel_24 = 24,
	kDMAChannel_25 = 25,
	kDMAChannel_26 = 26,
	kDMAChannel_27 = 27,
	kDMAChannel_28 = 28,
	kDMAChannel_29 = 29,
	kDMAChannel_30 = 30,
	kDMAChannel_31 = 31,
} DMAChannel;

typedef enum {
	kDMASource_0 = 0,
	kDMASource_1 = 1,
	kDMASource_2 = 2,
	kDMASource_3 = 3,
	kDMASource_Software = 4,
} DMASource;

typedef enum {
	kDMAArbitrationSize_1 = 0,
	kDMAArbitrationSize_2 = 1,
	kDMAArbitrationSize_4 = 2,
	kDMAArbitrationSize_8 = 3,
	kDMAArbitrationSize_16 = 4,
	kDMAArbitrationSize_32 = 5,
	kDMAArbitrationSize_64 = 6,
	kDMAArbitrationSize_128 = 7,
	kDMAArbitrationSize_256 = 8,
	kDMAArbitrationSize_512 = 9,
	kDMAArbitrationSize_1024 = 10,
} DMAArbitrationSize;

typedef enum {
	kDMAPacketSize_1 = 0,
	kDMAPacketSize_2 = 1,
	kDMAPacketSize_4 = 2,
} DMAPacketSize;

typedef enum {
	kDMAStride_0 = 1,
	kDMAStride_1 = 2,
	kDMAStride_2 = 3,
	kDMAStride_4 = 0
} DMAStride;

extern DMAControlChannel __dma_control_channels[64];

/*
dma_enable
----------------------
enables the DMA module
*/
static inline void dma_enable() {
	uint32_t rcgc_old = read_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCDMA);
	write_hw_u32((void *) SYSCTL_BASE, SYSCTL_RCGCDMA, rcgc_old | 1);
	for (volatile int i = 0; i < 2; i ++);
	uint32_t cfg_old = read_hw_u32((void *) DMA_BASE, DMA_CFG);
	write_hw_u32((void *) DMA_BASE, DMA_CFG, cfg_old | 1);
	uint32_t ctlbase_old = read_hw_u32((void *) DMA_BASE, DMA_CTLBASE);
	write_hw_u32((void *) DMA_BASE, DMA_CTLBASE, (ctlbase_old & 0xFF) | (uint32_t) __dma_control_channels);
	write_hw_u32((void *) DMA_BASE, DMA_ALTBASE, (uint32_t) & __dma_control_channels[32]);
}

static inline void dma_channel_set_priority(DMAChannel channel, bool high_priority) {
	if (high_priority)
		write_hw_u32((void *) DMA_BASE, DMA_PRIOSET, 1 << channel);
	else
		write_hw_u32((void *) DMA_BASE, DMA_PRIOCLEAR, 1 << channel);
}

static inline void dma_channel_select_primary(DMAChannel channel, bool primary) {
	if (primary)
		write_hw_u32((void *) DMA_BASE, DMA_ALTCLEAR, 1 << channel);
	else
		write_hw_u32((void *) DMA_BASE, DMA_ALTSET, 1 << channel);
}

static inline bool dma_channel_waiting(DMAChannel channel) {
	return read_hw_u32((void *) DMA_BASE, DMA_WAITSTATE) & (1 << channel);
}

static inline void dma_channel_software_request(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_SWREQ, 1 << channel);
}

static inline void dma_channel_software_requests_enable(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_REQMASKSET, 1 << channel);
}

static inline void dma_channel_peripheral_requests_enable(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_REQMASKCLR, 1 << channel);
}

static inline void dma_channel_enable(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_ENASET, 1 << channel);
}

static inline void dma_channel_disable(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_ENACLR, 1 << channel);
}

static inline void dma_channel_disable_single_requests(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_USEBURSTSET, 1 << channel);
}

static inline void dma_channel_enable_single_requests(DMAChannel channel) {
	write_hw_u32((void *) DMA_BASE, DMA_USEBURSTCLR, 1 << channel);
}

extern uintptr_t __dma_channel_regs[];

static inline void dma_channel_select_source(DMAChannel channel, DMASource source) {
	uint32_t chmap_old = read_hw_u32((void *) DMA_BASE, __dma_channel_regs[channel >> 3]);
	uint32_t bit_shift = ((channel & 0x7) << 2);
	write_hw_u32((void *) DMA_BASE, __dma_channel_regs[channel >> 3], (chmap_old & ~(0xF << bit_shift)) | (((uint32_t) source) << bit_shift));
}

static inline void dma_channel_stop(DMAChannel channel) {
	__dma_control_channels[channel].control = (__dma_control_channels[channel].control & ~ DMA_CONTROL_TRANSFER_MODE_MASK) | DMA_CONTROL_TRANSFER_MODE_STOP;
}

static inline bool dma_channel_finished(DMAChannel channel) {
	return (__dma_control_channels[channel].control & DMA_CONTROL_TRANSFER_MODE_MASK) == DMA_CONTROL_TRANSFER_MODE_STOP;
}

static inline void dma_channel_setup_basic(DMAChannel channel, void * source, void * dest, DMAPacketSize size, uint16_t transfer_size, DMAStride source_stride, DMAStride dest_stride, DMAArbitrationSize arbitration_size) {
	uint32_t control = DMA_CONTROL_TRANSFER_MODE_BASIC;
	if (transfer_size > 1024)
		transfer_size = 1024;
	transfer_size --;
	control |= transfer_size << DMA_CONTROL_TRANSFER_SIZE_SHIFT;
	control |= ((uint32_t) arbitration_size) << DMA_CONTROL_ARBITRATION_SIZE_SHIFT;
	control |= ((uint32_t) transfer_size) << DMA_CONTROL_SOURCE_SIZE_SHIFT;
	control |= ((uint32_t) source_stride) << DMA_CONTROL_SOURCE_INCREMENT_SHIFT;
	control |= ((uint32_t) transfer_size) << DMA_CONTROL_DESTINATION_SIZE_SHIFT;
	control |= ((uint32_t) dest_stride) << DMA_CONTROL_DESTINATION_INCREMENT_SHIFT;
	__dma_control_channels[channel].control = control;
}

static inline void dma_channel_setup_auto(DMAChannel channel, void * source, void * dest, DMAPacketSize size, uint16_t transfer_size, DMAStride source_stride, DMAStride dest_stride, DMAArbitrationSize arbitration_size) {
	__dma_control_channels[channel].control = (__dma_control_channels[channel].control & ~ DMA_CONTROL_TRANSFER_MODE_MASK) | DMA_CONTROL_TRANSFER_MODE_STOP;
	uint32_t control = DMA_CONTROL_TRANSFER_MODE_AUTO;
	uint32_t transfer_size_temp = transfer_size;
	if (transfer_size_temp > 1024)
		transfer_size_temp = 1024;
	transfer_size_temp --;
	uint32_t packet_size = 1 << size;
	control |= transfer_size_temp << DMA_CONTROL_TRANSFER_SIZE_SHIFT;
	control |= ((uint32_t) arbitration_size) << DMA_CONTROL_ARBITRATION_SIZE_SHIFT;
	control |= ((uint32_t) packet_size) << DMA_CONTROL_SOURCE_SIZE_SHIFT;
	control |= ((uint32_t) source_stride) << DMA_CONTROL_SOURCE_INCREMENT_SHIFT;
	control |= ((uint32_t) packet_size) << DMA_CONTROL_DESTINATION_SIZE_SHIFT;
	control |= ((uint32_t) dest_stride) << DMA_CONTROL_DESTINATION_INCREMENT_SHIFT;
	switch(source_stride) {
		case kDMAStride_0: {
			__dma_control_channels[channel].source_end = source;
		} break;
		
		case kDMAStride_1: {
			if (packet_size != 1)
				return;
			__dma_control_channels[channel].source_end = (void *) (((uintptr_t) source) + (uintptr_t) transfer_size - 1);
		} break;
		
		case kDMAStride_2: {
			if (packet_size == 4)
				return;
			__dma_control_channels[channel].source_end = (void *) (((uintptr_t) source) + ((uintptr_t) transfer_size) * 2 - 1);
		} break;
		
		case kDMAStride_4: {
			__dma_control_channels[channel].source_end = (void *) (((uintptr_t) source) + ((uintptr_t) transfer_size) * 4 - 1);
		} break;
	}
	switch(dest_stride) {
		case kDMAStride_0: {
			__dma_control_channels[channel].destination_end = dest;
		} break;
		
		case kDMAStride_1: {
			if (packet_size != 1)
				return;
			__dma_control_channels[channel].destination_end = (void *) (((uintptr_t) dest) + (uintptr_t) transfer_size - 1);
		} break;
		
		case kDMAStride_2: {
			if (packet_size == 4)
				return;
			__dma_control_channels[channel].destination_end = (void *) (((uintptr_t) dest) + ((uintptr_t) transfer_size) * 2 - 1);
		} break;
		
		case kDMAStride_4: {
			__dma_control_channels[channel].destination_end = (void *) (((uintptr_t) dest) + ((uintptr_t) transfer_size) * 4 - 1);
		} break;
	}
	__dma_control_channels[channel].control = control;
}

#endif
