#include <stdint.h>
#include <gpu.h>
#include <dma.h>
#include <region.h>
#include <interrupts.h>


static uint16_t current_draw_mode = 0;

static bool gpu_up = false;

uint32_t s_clear_color = 0x000000;

bool gpu_init(void){

    if(gpu_up){
        return true;
    }

    GP1_RESET();

    set_interrupt_channel(I_MASK_GPU,true);

    GP0_DRAW_MODE(GPU_DITHER);
    current_draw_mode = GPU_DITHER;
   

    enable_dma_channel(DMA_CH_GPU);
    enable_dma_channel(DMA_CH_OTC);
    GP1_DMA_DIR(2);

    GP1_DISPLAY_MODE(SCREEN_H_MODE|SCREEN_V_MODE|REGION_VIDEO_MODE|GPU_15BIT);
    GP1_DISPLAY_RANGE_H(DISPLAY_START_X,DISPLAY_START_X+(SCREEN_W*8));
    GP1_DISPLAY_RANGE_V(DISPLAY_CENTER_Y-(SCREEN_H/2),DISPLAY_CENTER_Y+(SCREEN_H/2));

    GP1_DISPLAY_START(F_BUFF_X0, F_BUFF_Y0);

    GP0_DRAW_AREA_TL(F_BUFF_X1,F_BUFF_Y1);
    GP0_DRAW_AREA_BR(F_BUFF_X1 + SCREEN_W - 1, F_BUFF_Y1 + SCREEN_H - 1);
    GP0_DRAW_OFFSET(F_BUFF_X1, F_BUFF_Y1);


    GP0_FILL(s_clear_color & 0x00FFFFFF, GPU_VRAM_XY(F_BUFF_X0, F_BUFF_Y0), GPU_XY(SCREEN_W, SCREEN_H));
    GPU_WAIT_CMD_READY();
    GP0_FILL(s_clear_color & 0x00FFFFFF, GPU_VRAM_XY(F_BUFF_X1, F_BUFF_Y1), GPU_XY(SCREEN_W, SCREEN_H));
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

void execute_sprite_draw(uint32_t command, uint32_t xy, uint32_t clut_uv, uint32_t wh, uint16_t texpage){

    if(!gpu_init()){
        return;
    }


    GPU_WAIT_CMD_READY();

    uint16_t mode = texpage|GPU_DITHER;

    if(current_draw_mode!=mode){
        GP0_DRAW_MODE(mode);
        current_draw_mode=mode;
    }

    GP0_WRITE(command);
    GP0_WRITE(xy);
    GP0_WRITE(clut_uv);
    GP0_WRITE(wh);

}

void execute_scene_draw(uint32_t *ot, size_t count, uint16_t x, uint16_t y){

    if(!gpu_init()){
        return;
    }

    GPU_WAIT_CMD_READY();
    GP0_FILL(s_clear_color & 0x00FFFFFF,GPU_VRAM_XY(x,y),GPU_XY(SCREEN_W,SCREEN_H));
    GPU_WAIT_CMD_READY();

    start_dma_transfer(DMA_CH_GPU, &ot[count-1], 0, DMA_TO_DEVICE | DMA_SYNC_LIST | DMA_START, false);


}


void gpu_irq(void){
    GP1_ACK_IRQ();
}