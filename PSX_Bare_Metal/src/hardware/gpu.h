#ifndef GPU_H
#define GPU_H

#include <mmio.h>

#define GPU_VRAM_COLUMNS(w,bpp)((((w) + (1 << (bpp)) - 1) >> (bpp)))

#define GP0_WRITE(value){_MMIO32(0x1F801810)=(value);}
#define GP1_WRITE(value){_MMIO32(0x1F801814)=(value);}

#define GPUREAD() (_MMIO32(0x1F801810))
#define GPUSTAT() (_MMIO32(0x1F801814))

#define GP1_RESET(){GP1_WRITE(0x00000000);}
#define GP1_RESET_FIFO(){GP1_WRITE(0x01000000);}
#define GP1_ACK_IRQ(){GP1_WRITE(0x02000000);}
#define GP1_DISPLAY_ENABLE(state){GP1_WRITE(0x03000000|((state)?0x0:0x1));}
#define GP1_DMA_DIR(dir){GP1_WRITE(0x04000000|((dir)&0x3));}
#define GP1_DISPLAY_START(x,y){GP1_WRITE(0x05000000|(((y)&0x1FF)<<10)|((x)&0x3FF));}
#define GP1_DISPLAY_RANGE_H(x1,x2){GP1_WRITE(0x06000000|(((x2)&0xFFF)<<12)|((x1)&0xFFF));}
#define GP1_DISPLAY_RANGE_V(y1,y2){GP1_WRITE(0x07000000|(((y2)&0x3FF)<<10)|((y1)&0x3FF));}
#define GP1_DISPLAY_MODE(value){GP1_WRITE(0x08000000|((value)&0xFFFFFF));}

#define GP0_NOP(){GP0_WRITE(0x00000000);}
#define GP0_FLUSH_TEX_CACHE(){GP0_WRITE(0x01000000);}
#define GP0_FILL(rgb,dest,size)do{GP0_WRITE(0x02000000|((rgb)&0xFFFFFF));GP0_WRITE((dest));GP0_WRITE((size));}while(false)

#define GP0_DRAW_MODE(value){GP0_WRITE(0xE1000000|((value)&0x3FFF));}

#define GP0_TEX_WINDOW(value){GP0_WRITE(0xE2000000|((value)&0xFFFFF));}
#define GP0_DRAW_AREA_TL(x,y){GP0_WRITE(0xE3000000|(((y)&0x3FF)<<10)|((x)&0x3FF));}
#define GP0_DRAW_AREA_BR(x,y){GP0_WRITE(0xE4000000|(((y)&0x3FF)<<10)|((x)&0x3FF));}
#define GP0_DRAW_OFFSET(x,y){GP0_WRITE(0xE5000000|(((y)&0x7FF)<<11)|((x)&0x7FF));}
#define GP0_MASK_BIT(value){GP0_WRITE(0xE6000000|((value)&0x3));}

#define GP0_VRAM_WRITE_BEGIN(dest,size)do{GP0_WRITE(0xA0000000);GP0_WRITE((dest));GP0_WRITE((size));}while(false)
#define GP0_VRAM_WRITE_WORD(value){GP0_WRITE((value));}

#define GPU_CMD_BUSY() ((GPUSTAT()&0x04000000)==0)
#define GPU_VRAM_READ_READY() ((GPUSTAT()&0x08000000)!=0)
#define GPU_DMA_READY() ((GPUSTAT()&0x10000000)!=0)

#define GPU_WAIT_CMD_READY()do{while(GPU_CMD_BUSY());}while(false)
#define GPU_WAIT_DMA_READY()do{while(!GPU_DMA_READY());}while(false)

#define GPU_XY(x,y) ((uint32_t)((((uint32_t)(y)&0xFFFF)<<16)|((uint32_t)(x)&0xFFFF)))
#define GPU_UV(u,v) ((uint32_t)((((uint32_t)(v)&0xFF)<<8)|((uint32_t)(u)&0xFF)))

#define GPU_VRAM_XY(x,y) ((((uint32_t)(y) & 0x1FF) << 16) | ((uint32_t)(x) & 0x3FF))

typedef enum gp1_display_mode : uint32_t {
    GPU_H256 = 0x00000000,
    GPU_H320 = 0x00000001,
    GPU_H512 = 0x00000002,
    GPU_H640 = 0x00000003,
    GPU_H368 = 0x00000040,
    GPU_V240 = 0x00000000,
    GPU_V480 = 0x00000004,
    GPU_NTSC = 0x00000000,
    GPU_PAL = 0x00000008,
    GPU_15BIT = 0x00000000,
    GPU_24BIT = 0x00000010,
    GPU_INTERLACE = 0x00000020
} GPU_DISPLAY_MODE;

typedef enum gp0_draw_mode : uint32_t {
    GPU_TEX_4BIT = 0x0000,
    GPU_TEX_8BIT = 0x0080,
    GPU_TEX_15BIT = 0x0100,
    GPU_BLEND_AVERAGE = 0x0000,
    GPU_BLEND_ADD = 0x0020,
    GPU_BLEND_SUB = 0x0040,
    GPU_BLEND_ADD_QUARTER = 0x0060,
    GPU_DITHER = 0x0200,
    GPU_DRAW_TO_DISPLAY = 0x0400,
    GPU_TEX_DISABLE = 0x0800
} GPU_DRAW_MODE;

typedef enum gpu_status : uint32_t {
    GPUSTAT_IRQ = 0x01000000,
    GPUSTAT_DMA_REQ = 0x02000000,
    GPUSTAT_CMD_READY = 0x04000000,
    GPUSTAT_VRAM_READ_READY = 0x08000000,
    GPUSTAT_DMA_READY = 0x10000000,
    GPUSTAT_DISP_OFF = 0x00800000,
    GPUSTAT_INTERLACE_FIELD = 0x00002000,
    GPUSTAT_ODD_LINE = 0x80000000
} GPU_STATUS;

typedef enum gpu_prim_flag : uint32_t {
    GPU_PRIM_RAW      = 0x01000000,
    GPU_PRIM_SEMI     = 0x02000000,
    GPU_PRIM_TEXTURED = 0x04000000
} GPU_PRIM_FLAG;

typedef enum gpu_poly_flag : uint32_t {
    GPU_POLY_QUAD     = 0x08000000,
    GPU_POLY_GOURAUD  = 0x10000000
} GPU_POLY_FLAG;

typedef enum gpu_line_flag : uint32_t {
    GPU_LINE_POLYLINE = 0x08000000,
    GPU_LINE_GOURAUD  = 0x10000000
} GPU_LINE_FLAG;

typedef enum gpu_rect_size : uint32_t {
    GPU_RECT_VAR      = 0x00000000,
    GPU_RECT_1        = 0x08000000,
    GPU_RECT_8        = 0x10000000,
    GPU_RECT_16       = 0x18000000
} GPU_RECT_SIZE;

typedef struct gpu_sprite {
    uint32_t cmd;
    uint32_t xy;
    uint32_t clut_uv;
    uint32_t wh;
} GPU_SPRITE;

typedef enum gpu_bpp : uint8_t {
    GPU_DIRECT = 0x0,
    GPU_8BPP = 0x1,
    GPU_4BPP = 0x2
} GPU_BPP;

void gpu_irq(void);

bool gpu_init(void);

bool gpu_upload(uint32_t dest, uint32_t size, const void *buffer, uint32_t halfwords);

void execute_sprite_draw(uint32_t command, uint32_t xy, uint32_t clut_uv, uint32_t wh, uint16_t texpage);

#endif