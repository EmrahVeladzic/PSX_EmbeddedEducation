#include <scene.h>
#include <hardware.h>

int main(void){

    set_screen_offset(FIX_SHL(SCREEN_W/2,16),FIX_SHL(SCREEN_H/2,16),PROJ_DIST,Z_SCALE_FACTOR);

    SPU_KEY_OFF(0);
   
    setup();

    int last_frame = get_frame();

    set_interrupt_channel(I_MASK_VBLANK,true);



    for (int f = last_frame;; f = get_frame()){
        
        tick(last_frame);
        last_frame = f;

        DMA_SYNC(DMA_CH_GPU);
        GPU_WAIT_CMD_READY();

        release = false;

        while (!release)
        {
            __asm__ volatile ("");
        }
        
        uint16_t draw_x = second_framebuffer ? F_BUFF_X0 : F_BUFF_X1;
        uint16_t draw_y = second_framebuffer ? F_BUFF_Y0 : F_BUFF_Y1;
        uint16_t show_x = second_framebuffer ? F_BUFF_X1 : F_BUFF_X0;
        uint16_t show_y = second_framebuffer ? F_BUFF_Y1 : F_BUFF_Y0;

        GP1_DISPLAY_START(show_x, show_y);

        GP0_DRAW_AREA_TL(draw_x, draw_y);
        GP0_DRAW_AREA_BR(draw_x + SCREEN_W - 1, draw_y + SCREEN_H - 1);
        GP0_DRAW_OFFSET(draw_x, draw_y);

        submit_scene();

        second_framebuffer=!second_framebuffer;
    }
    

    return 0;
}