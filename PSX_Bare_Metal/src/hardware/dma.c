#include <dma.h>
#include <mmio.h>
#include <interrupts.h>
#include <hardware.h>


void enable_dma_channel(DMA_CHANNEL ch){
    _MMIO32(DMA_DPCR)|=DMA_DPCR_MASK(ch);
    DMA_IRQ_CH_ON(ch);
}
void disable_dma_channel(DMA_CHANNEL ch){
    _MMIO32(DMA_DPCR)&=~DMA_DPCR_MASK(ch);
    DMA_IRQ_CH_OFF(ch);
}

void start_dma_transfer(DMA_CHANNEL ch, void* address, uint32_t bcr, DMA_CHCR_FLAGS flags, bool sync) {
    DMA_MASTER_ARM_ON;
    set_interrupt_channel(I_MASK_DMA,true);
    _MMIO32(DMA_MADR(ch)) = (uint32_t)address;
    _MMIO32(DMA_BCR(ch))  = bcr;
    _MMIO32(DMA_CHCR(ch)) = flags;
    if(sync){
        DMA_SYNC(ch);
    }
}

void dma_irq(void){
    uint32_t pending = DMA_PENDING();
    for (int ch = 0; ch < 7; ch++){
        if (pending & (1u << ch)){
            DMA_ACK(ch);  
        }
    }
    ram_debug("DMA_IRQ");
}