#include <dma.h>

void toggle_dma_channel(DMA_CHANNEL channel, uint32_t state){
    if(state){
        _MMIO32(DMA_DPCR)|=channel;
    }
    else{
       _MMIO32(DMA_DPCR)&=~(channel);
    }
}