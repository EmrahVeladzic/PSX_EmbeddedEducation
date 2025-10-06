#ifndef DMA_H
#define DMA_H

typedef enum {
    MDEC_I  = 0x8,
    MDEC_O  = 0x8 << 4,
    GPU     = 0x8 << 8,
    CDROM   = 0x8 << 12,
    SPU     = 0x8 << 16,
    PIO     = 0x8 << 20,
    GPU_OTC = 0x8 << 24,
    ALL     = 0x08888888
} DMA_CHANNEL;


#define DMA_DPCR 0x1F8010F0

void toggle_dma_channel(DMA_CHANNEL channel, int state);

#endif