#ifndef TEXTURE_H
#define TEXTURE_H

#include <gpu.h>

#define VRAM_X(p) ((p) & 0xFFFF)
#define VRAM_Y(p) (((p) >> 16) & 0x1FF)
#define VRAM_W(s) ((s) & 0xFFFF)
#define VRAM_H(s) ((s) >> 16)

typedef struct gpu_mem {
    int32_t dest_tex;
    int32_t size_tex;
    int32_t dest_clut;
    int32_t size_clut; 
    uint16_t texpage;
    uint16_t clut_word;
    struct gpu_mem *next;
    struct gpu_mem *prev;   
    char *path;
} GPU_Allocation;


GPU_Allocation *load_texture(const char *path);

void clear_texture(GPU_Allocation *texture);

void draw_sprite(GPU_Allocation *texture, GPU_PRIM_FLAG flag, int16_t x, int16_t y, uint8_t u, uint8_t v, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b);

#endif
