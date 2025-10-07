#ifndef DMA_H
#define DMA_H

typedef enum dma_c{
    DMA_MDEC_I  = 0x8,
    DMA_MDEC_O  = 0x8 << 4,
    DMA_GPU     = 0x8 << 8,
    DMA_CDROM   = 0x8 << 12,
    DMA_SPU     = 0x8 << 16,
    DMA_PIO     = 0x8 << 20,
    DMA_GPU_OTC = 0x8 << 24,
    DMA_ALL     = 0x08888888
} DMA_CHANNEL;

#define DMA_DPCR 0x1F8010F0

void toggle_dma_channel(DMA_CHANNEL channel, uint32_t state);

#endif