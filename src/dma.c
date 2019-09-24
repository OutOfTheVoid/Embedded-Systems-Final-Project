#include "dma.h"

uintptr_t __dma_channel_regs[] = {
	DMA_CHMAP0,
	DMA_CHMAP1,
	DMA_CHMAP2,
	DMA_CHMAP3,
};

DMAControlChannel __attribute__ ((aligned (1024))) __dma_control_channels[64];
