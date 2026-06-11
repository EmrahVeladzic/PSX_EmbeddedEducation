#include <dma.h>
#include <mmio.h>


void enable_dma_channel(DMA_CHANNEL ch){
    _MMIO32(DMA_DPCR)|=DMA_DPCR_MASK(ch);
    _MMIO32(DMA_DICR)|=(0x1<<(ch+16));
}
void disable_dma_channel(DMA_CHANNEL ch){
    _MMIO32(DMA_DPCR)&=~DMA_DPCR_MASK(ch);
    _MMIO32(DMA_DICR)&=~(0x1<<(ch+16));
}

void start_dma_transfer(DMA_CHANNEL ch, void* address, uint32_t bcr, DMA_CHCR_FLAGS flags) {
    _MMIO32(DMA_MADR(ch)) = (uint32_t)address;
    _MMIO32(DMA_BCR(ch))  = bcr;
    _MMIO32(DMA_CHCR(ch)) = flags;
    DMA_SYNC(ch);
}

void dma_irq(void){
  
   
   
}