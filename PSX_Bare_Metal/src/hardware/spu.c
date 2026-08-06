#include <spu.h>
#include <interrupts.h>
#include <dma.h>

bool spu_up = false;

void spu_init(void){
    if(spu_up){return;}

    set_interrupt_channel(I_MASK_SPU,true);
    enable_dma_channel(DMA_CH_SPU);

    SPU_CNT_WRITE(SPU_OFF);
    while ((SPU_STAT_READ() & SPUSTAT_MODE_MASK)) {__asm__ volatile("");}

    SPU_XFER_CTRL(SPU_XFER_NORMAL);
    SPU_MASTER_VOLUME_BOTH(0x3FFF);

    for (int v = 0; v < 24; v++) {
        SPU_PMON(v, false);
        SPU_NON(v, false);
        SPU_EON(v, false);
    }

    for (int v = 0; v < 24; v++) {
        SPU_VOICE_VOLUME_BOTH(v, 0);
        SPU_VOICE_PITCH(v, 0);
        SPU_VOICE_ADSR(v, 0, 0);
    }

    _MMIO16(0x1F801D8C) = 0xFFFF;   /* KOFF voices 0-15  */
    _MMIO16(0x1F801D8E) = 0x00FF;   /* KOFF voices 16-23 */

    SPU_CNT_WRITE(SPU_ENABLE | SPU_DEMUTE);
    while ((SPU_STAT_READ() & SPUSTAT_MODE_MASK)) {__asm__ volatile("");}

    spu_up = true;

}


bool load_audio(void *buffer, uint32_t blocks, uint16_t sample_rate, uint8_t voice){
    if(!spu_up){spu_init();}

    SPU_XFER_ADDR(0x1000 >> 3);

    SPU_CNT_WRITE(SPU_ENABLE | SPU_DEMUTE | SPU_DMA_W);
    while ((SPU_STAT_READ() & SPUSTAT_MODE_MASK) != SPU_DMA_W) {__asm__ volatile("");}

    start_dma_transfer(DMA_CH_SPU, buffer, SPU_DMA_BCR(blocks), DMA_TO_DEVICE | DMA_SYNC_SLICE | DMA_START, true);

    while (SPU_STAT_READ() & SPUSTAT_XFER_BUSY) {__asm__ volatile("");}

    SPU_CNT_WRITE(SPU_ENABLE | SPU_DEMUTE);
    while ((SPU_STAT_READ() & SPUSTAT_MODE_MASK) != 0) {__asm__ volatile("");}

    SPU_VOICE_START(voice, 0x1000 >> 3);
    SPU_VOICE_PITCH(voice, SPU_PITCH_FROM_RATE(sample_rate));
    SPU_VOICE_ADSR(voice, 0x00FF, 0x0000);
    SPU_VOICE_VOLUME_BOTH(voice, 0x3FFF);

    return true;

}

void spu_irq(void){
     uint16_t cnt = SPU_CNT_READ();
    SPU_CNT_WRITE(cnt & ~SPU_IRQ);   
    SPU_CNT_WRITE(cnt |  SPU_IRQ);  
}