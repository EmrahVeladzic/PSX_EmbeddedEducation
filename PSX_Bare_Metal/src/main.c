#include <stdlib.h>
#include <stdio.h>
#include <hardware.h>
#include <audio.h>
#include <texture.h>

int main(void){

    int8_t *array = malloc(48);



    const char *A = "AAAAA";
    const char *B = "BBBBB";  
    const char *C = "CCCCC";

    for (size_t i = 0; i < 48; i++){
        array[i]=A[0];
    }  

    array = realloc(array,24);

    delay_microseconds(5000000);

    int8_t *crray = malloc(8);

    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }

    for (size_t i = 0; i < 8; i++)
    {
        crray[i]=C[0];
    }

    SPU_KEY_OFF(0);
    SPU_Allocation *sfx = load_audio("ASSETS/RAVEN.WL",0);
    
    if(sfx){
        spu_play(0,sfx->origin,sfx->sample_rate);
    }


    GPU_Allocation *tex0 = load_texture("ASSETS/RAVEN.RPF");

   

    GPU_Allocation *tex1 = load_texture("ASSETS/SKELE.RPF");

   

    GPU_Allocation *tex2 = load_texture("ASSETS/FONT.RPF");

   
    GPU_Allocation *tex3 = load_texture("ASSETS/CHARA.RPF");

   
    if(tex0){
        draw_sprite(tex0,GPU_PRIM_TEXTURED|GPU_PRIM_RAW,0,0,0,0,128,128,0,0,0);
    }

    if(tex1){
        draw_sprite(tex1,GPU_PRIM_TEXTURED|GPU_PRIM_RAW,128,0,0,0,128,128,0,0,0);
    }

    if(tex3){
        draw_sprite(tex3,GPU_PRIM_TEXTURED|GPU_PRIM_RAW,0,128,0,0,128,128,0,0,0);
    }
   

    while (1) {
        __asm__ volatile("");


    }

    return 0;
}