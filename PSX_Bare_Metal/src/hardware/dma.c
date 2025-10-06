#include <dma.h>

void toggle_dma_channel(DMA_CHANNEL channel, int state){
    if(state){
        *(volatile int *)DMA_DPCR|=channel;
    }
    else{
        *(volatile int *)DMA_DPCR&=~(channel);
    }
}