#include <stdint.h>
#include <gpu.h>
#include <dma.h>
#include <region.h>
#include <interrupts.h>


static bool gpu_up = false;

bool gpu_init(void){

    if(gpu_up){
        return true;
    }

    GP1_RESET();

    set_interrupt_channel(I_MASK_GPU,true);
    

    enable_dma_channel(DMA_CH_GPU);
    GP1_DMA_DIR(2);

    GP1_DISPLAY_MODE(SCREEN_H_MODE|SCREEN_V_MODE|REGION_VIDEO_MODE|GPU_15BIT);
    GP1_DISPLAY_RANGE_H(DISPLAY_START_X,DISPLAY_START_X+(SCREEN_W*8));
    GP1_DISPLAY_RANGE_V(DISPLAY_CENTER_Y-(SCREEN_H/2),DISPLAY_CENTER_Y+(SCREEN_H/2));

    GP1_DISPLAY_START(0,0);

    GP0_DRAW_AREA_TL(0,0);
    GP0_DRAW_AREA_BR(SCREEN_W-1,SCREEN_H-1);
    GP0_DRAW_OFFSET(0,0);

    GP0_FILL(0x000000,GPU_XY(0,0),GPU_XY(SCREEN_W,SCREEN_H));
    GPU_WAIT_CMD_READY();

    GP1_DISPLAY_ENABLE(true);

    gpu_up = true;

    return true;
}

bool gpu_upload(uint32_t dest, uint32_t size, const void *buffer, uint32_t halfwords){

    if(!gpu_init()){
        return false;
    }

    uint32_t words = halfwords >> 1;

    uint32_t block_size = (words & 0xF) ? 8 : 16; 

    uint32_t block_count = words / block_size;

    GPU_WAIT_CMD_READY();

    GP0_VRAM_WRITE_BEGIN(dest,size);

    GPU_WAIT_DMA_READY();

    start_dma_transfer(DMA_CH_GPU,(void*)buffer,(block_count<<16)|block_size,DMA_TO_DEVICE|DMA_SYNC_SLICE|DMA_START,true);

    GPU_WAIT_CMD_READY();

    GP0_FLUSH_TEX_CACHE();

    return true;
}

void gpu_irq(void){
    GP1_ACK_IRQ();
}