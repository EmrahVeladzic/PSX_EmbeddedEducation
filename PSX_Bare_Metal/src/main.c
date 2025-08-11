/*
 * ps1-bare-metal - (C) 2023-2025 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * In this tutorial we're going to initialize the GPU, set it up and display
 * some simple hardware-rendered graphics (namely, a shaded triangle).
 *
 * While the PS1's GPU may appear complicated and daunting, its principle of
 * operation is in actual fact very simple. At a high level it is simply a
 * "rasterization machine" capable of drawing triangles, quads, rectangles and
 * lines in 2D space (with 3D transformations being entirely the CPU's
 * responsibility) and of displaying a rectangular cutout of its 1024x512x16bpp
 * framebuffer, which resides in dedicated memory (VRAM). It is controlled using
 * only two registers, one (GP0) for drawing commands and the other (GP1) for
 * display control and other commands; we're going to see how to configure the
 * video output and draw our triangle by writing the right commands to these
 * registers.
 *
 * This tutorial will use the ps1/gpucmd.h header file I wrote, which contains
 * enumerations and inline functions for all commands supported by the GPU. If
 * you wish to write such a header yourself, you'll want to check out the
 * GPU register and command documentation at:
 *     https://psx-spx.consoledev.net/graphicsprocessingunitgpu
 */

/*
 * ps1-bare-metal - (C) 2023-2025 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


#include <stdint.h>

#define _ADDR8(addr)  ((volatile uint8_t *) (addr))
#define _ADDR16(addr) ((volatile uint16_t *) (addr))
#define _ADDR32(addr) ((volatile uint32_t *) (addr))
#define _MMIO8(addr)  (*_ADDR8(addr))
#define _MMIO16(addr) (*_ADDR16(addr))
#define _MMIO32(addr) (*_ADDR32(addr))

/* Constants */

#define F_CPU      33868800
#define F_GPU_NTSC 53693175
#define F_GPU_PAL  53203425

typedef enum {
	DEV0_BASE  = 0xbf000000,
	EXP1_BASE  = 0xbf000000,
	CACHE_BASE = 0x9f800000, // Cannot be accessed from KSEG1
	IO_BASE    = 0xbf801000,
	EXP2_BASE  = 0xbf802000,
	EXP3_BASE  = 0xbfa00000,
	DEV2_BASE  = 0xbfc00000,
	CPU_BASE   = 0xfffe0000
} BaseAddress;

/* Bus interface */

typedef enum {
	BIU_CTRL_WRITE_DELAY_BITMASK = 15 <<  0,
	BIU_CTRL_READ_DELAY_BITMASK  = 15 <<  4,
	BIU_CTRL_RECOVERY            =  1 <<  8,
	BIU_CTRL_HOLD                =  1 <<  9,
	BIU_CTRL_FLOAT               =  1 << 10,
	BIU_CTRL_PRESTROBE           =  1 << 11,
	BIU_CTRL_WIDTH_8             =  0 << 12,
	BIU_CTRL_WIDTH_16            =  1 << 12,
	BIU_CTRL_AUTO_INCR           =  1 << 13,
	BIU_CTRL_SIZE_BITMASK        = 31 << 16,
	BIU_CTRL_DMA_DELAY_BITMASK   = 15 << 24,
	BIU_CTRL_ADDR_ERROR          =  1 << 28,
	BIU_CTRL_DMA_DELAY           =  1 << 29,
	BIU_CTRL_DMA32               =  1 << 30,
	BIU_CTRL_WAIT                =  1 << 31
} BIUControlFlag;

#define BIU_DEV0_ADDR _MMIO32(IO_BASE | 0x000) // PIO/573
#define BIU_EXP2_ADDR _MMIO32(IO_BASE | 0x004) // PIO/debug
#define BIU_DEV0_CTRL _MMIO32(IO_BASE | 0x008) // PIO/573
#define BIU_EXP3_CTRL _MMIO32(IO_BASE | 0x00c) // PIO/debug
#define BIU_DEV2_CTRL _MMIO32(IO_BASE | 0x010) // BIOS ROM
#define BIU_DEV4_CTRL _MMIO32(IO_BASE | 0x014) // SPU
#define BIU_DEV5_CTRL _MMIO32(IO_BASE | 0x018) // CD-ROM
#define BIU_EXP2_CTRL _MMIO32(IO_BASE | 0x01c) // PIO/debug
#define BIU_COM_DELAY _MMIO32(IO_BASE | 0x020)

/* Serial interfaces */

typedef enum {
	SIO_STAT_TX_NOT_FULL   = 1 << 0,
	SIO_STAT_RX_NOT_EMPTY  = 1 << 1,
	SIO_STAT_TX_EMPTY      = 1 << 2,
	SIO_STAT_RX_PARITY_ERR = 1 << 3,
	SIO_STAT_RX_OVERRUN    = 1 << 4, // SIO1 only
	SIO_STAT_RX_STOP_ERR   = 1 << 5, // SIO1 only
	SIO_STAT_RX_INVERT     = 1 << 6, // SIO1 only
	SIO_STAT_DSR           = 1 << 7, // DSR is /ACK on SIO0
	SIO_STAT_CTS           = 1 << 8, // SIO1 only
	SIO_STAT_IRQ           = 1 << 9
} SIOStatusFlag;

typedef enum {
	SIO_MODE_BAUD_BITMASK   = 3 << 0,
	SIO_MODE_BAUD_DIV1      = 1 << 0,
	SIO_MODE_BAUD_DIV16     = 2 << 0,
	SIO_MODE_BAUD_DIV64     = 3 << 0,
	SIO_MODE_DATA_BITMASK   = 3 << 2,
	SIO_MODE_DATA_5         = 0 << 2,
	SIO_MODE_DATA_6         = 1 << 2,
	SIO_MODE_DATA_7         = 2 << 2,
	SIO_MODE_DATA_8         = 3 << 2,
	SIO_MODE_PARITY_BITMASK = 3 << 4,
	SIO_MODE_PARITY_NONE    = 0 << 4,
	SIO_MODE_PARITY_EVEN    = 1 << 4,
	SIO_MODE_PARITY_ODD     = 3 << 4,
	SIO_MODE_STOP_BITMASK   = 3 << 6, // SIO1 only
	SIO_MODE_STOP_1         = 1 << 6, // SIO1 only
	SIO_MODE_STOP_1_5       = 2 << 6, // SIO1 only
	SIO_MODE_STOP_2         = 3 << 6, // SIO1 only
	SIO_MODE_SCK_INVERT     = 1 << 8  // SIO0 only
} SIOModeFlag;

typedef enum {
	SIO_CTRL_TX_ENABLE      = 1 <<  0,
	SIO_CTRL_DTR            = 1 <<  1, // DTR is /CS on SIO0
	SIO_CTRL_RX_ENABLE      = 1 <<  2,
	SIO_CTRL_TX_INVERT      = 1 <<  3, // SIO1 only
	SIO_CTRL_ACKNOWLEDGE    = 1 <<  4,
	SIO_CTRL_RTS            = 1 <<  5, // SIO1 only
	SIO_CTRL_RESET          = 1 <<  6,
	SIO_CTRL_TX_IRQ_ENABLE  = 1 << 10,
	SIO_CTRL_RX_IRQ_ENABLE  = 1 << 11,
	SIO_CTRL_DSR_IRQ_ENABLE = 1 << 12, // DSR is /ACK on SIO0
	SIO_CTRL_CS_PORT_1      = 0 << 13, // SIO0 only
	SIO_CTRL_CS_PORT_2      = 1 << 13  // SIO0 only
} SIOControlFlag;

// SIO_DATA is a 32-bit register, but some emulators do not implement it
// correctly and break if it's read more than 8 bits at a time.
#define SIO_DATA(N) _MMIO8 ((IO_BASE | 0x040) + (16 * (N)))
#define SIO_STAT(N) _MMIO16((IO_BASE | 0x044) + (16 * (N)))
#define SIO_MODE(N) _MMIO16((IO_BASE | 0x048) + (16 * (N)))
#define SIO_CTRL(N) _MMIO16((IO_BASE | 0x04a) + (16 * (N)))
#define SIO_BAUD(N) _MMIO16((IO_BASE | 0x04e) + (16 * (N)))

/* DRAM controller */

typedef enum {
	DRAM_CTRL_UNKNOWN     = 1 <<  3,
	DRAM_CTRL_FETCH_DELAY = 1 <<  7,
	DRAM_CTRL_SIZE_MUL1   = 0 <<  9,
	DRAM_CTRL_SIZE_MUL4   = 1 <<  9,
	DRAM_CTRL_COUNT_1     = 0 << 10, // 1 DRAM bank (single RAS)
	DRAM_CTRL_COUNT_2     = 1 << 10, // 2 DRAM banks (dual RAS)
	DRAM_CTRL_SIZE_1MB    = 0 << 11, // 1MB chips (4MB with MUL4)
	DRAM_CTRL_SIZE_2MB    = 1 << 11  // 2MB chips (8MB with MUL4)
} DRAMControlFlag;

#define DRAM_CTRL _MMIO32(IO_BASE | 0x060)

/* IRQ controller */

typedef enum {
	IRQ_VSYNC  =  0,
	IRQ_GPU    =  1,
	IRQ_CDROM  =  2,
	IRQ_DMA    =  3,
	IRQ_TIMER0 =  4,
	IRQ_TIMER1 =  5,
	IRQ_TIMER2 =  6,
	IRQ_SIO0   =  7,
	IRQ_SIO1   =  8,
	IRQ_SPU    =  9,
	IRQ_GUN    = 10,
	IRQ_PIO    = 10
} IRQChannel;

#define IRQ_STAT _MMIO16(IO_BASE | 0x070)
#define IRQ_MASK _MMIO16(IO_BASE | 0x074)

/* DMA */

typedef enum {
	DMA_MDEC_IN  = 0,
	DMA_MDEC_OUT = 1,
	DMA_GPU      = 2,
	DMA_CDROM    = 3,
	DMA_SPU      = 4,
	DMA_PIO      = 5,
	DMA_OTC      = 6
} DMAChannel;

typedef enum {
	DMA_CHCR_READ             = 0 <<  0,
	DMA_CHCR_WRITE            = 1 <<  0,
	DMA_CHCR_REVERSE          = 1 <<  1,
	DMA_CHCR_CHOPPING         = 1 <<  8,
	DMA_CHCR_MODE_BITMASK     = 3 <<  9,
	DMA_CHCR_MODE_BURST       = 0 <<  9,
	DMA_CHCR_MODE_SLICE       = 1 <<  9,
	DMA_CHCR_MODE_LIST        = 2 <<  9,
	DMA_CHCR_DMA_TIME_BITMASK = 7 << 16,
	DMA_CHCR_CPU_TIME_BITMASK = 7 << 20,
	DMA_CHCR_ENABLE           = 1 << 24,
	DMA_CHCR_TRIGGER          = 1 << 28,
	DMA_CHCR_PAUSE            = 1 << 29  // Burst mode only
} DMACHCRFlag;

#define DMA_DPCR_CH_PRIORITY_BITMASK(N)   (7              << (4 * (N)))
#define DMA_DPCR_CH_PRIORITY(N, priority) ((priority & 7) << (4 * (N)))
#define DMA_DPCR_CH_ENABLE(N)             ((1 << 3)       << (4 * (N)))

typedef enum {
	DMA_DICR_CH_MODE_BITMASK   = 0x7f <<  0,
	DMA_DICR_BUS_ERROR         =    1 << 15,
	DMA_DICR_CH_ENABLE_BITMASK = 0x7f << 16,
	DMA_DICR_IRQ_ENABLE        =    1 << 23,
	DMA_DICR_CH_STAT_BITMASK   = 0x7f << 24,
	DMA_DICR_IRQ               =    1 << 31
} DMADICRFlag;

#define DMA_DICR_CH_MODE(N)   (1 << ((N) +  0))
#define DMA_DICR_CH_ENABLE(N) (1 << ((N) + 16))
#define DMA_DICR_CH_STAT(N)   (1 << ((N) + 24))

#define DMA_MADR(N) _MMIO32((IO_BASE | 0x080) + (16 * (N)))
#define DMA_BCR(N)  _MMIO32((IO_BASE | 0x084) + (16 * (N)))
#define DMA_CHCR(N) _MMIO32((IO_BASE | 0x088) + (16 * (N)))

#define DMA_DPCR _MMIO32(IO_BASE | 0x0f0)
#define DMA_DICR _MMIO32(IO_BASE | 0x0f4)

/* Timers */

typedef enum {
	TIMER_CTRL_ENABLE_SYNC     = 1 <<  0,
	TIMER_CTRL_SYNC_BITMASK    = 3 <<  1,
	TIMER_CTRL_SYNC_PAUSE      = 0 <<  1,
	TIMER_CTRL_SYNC_RESET1     = 1 <<  1,
	TIMER_CTRL_SYNC_RESET2     = 2 <<  1,
	TIMER_CTRL_SYNC_PAUSE_ONCE = 3 <<  1,
	TIMER_CTRL_RELOAD          = 1 <<  3,
	TIMER_CTRL_IRQ_ON_RELOAD   = 1 <<  4,
	TIMER_CTRL_IRQ_ON_OVERFLOW = 1 <<  5,
	TIMER_CTRL_IRQ_REPEAT      = 1 <<  6,
	TIMER_CTRL_IRQ_LATCH       = 1 <<  7,
	TIMER_CTRL_EXT_CLOCK       = 1 <<  8,
	TIMER_CTRL_PRESCALE        = 1 <<  9,
	TIMER_CTRL_IRQ             = 1 << 10,
	TIMER_CTRL_RELOADED        = 1 << 11,
	TIMER_CTRL_OVERFLOWED      = 1 << 12
} TimerControlFlag;

#define TIMER_VALUE(N)  _MMIO16((IO_BASE | 0x100) + (16 * (N)))
#define TIMER_CTRL(N)   _MMIO16((IO_BASE | 0x104) + (16 * (N)))
#define TIMER_RELOAD(N) _MMIO16((IO_BASE | 0x108) + (16 * (N)))

/* CD-ROM drive */

typedef enum {
	CDROM_HSTS_RA_BITMASK = 3 << 0,
	CDROM_HSTS_ADPBUSY    = 1 << 2,
	CDROM_HSTS_PRMEMPT    = 1 << 3,
	CDROM_HSTS_PRMWRDY    = 1 << 4,
	CDROM_HSTS_RSLRRDY    = 1 << 5,
	CDROM_HSTS_DRQSTS     = 1 << 6,
	CDROM_HSTS_BUSYSTS    = 1 << 7
} CDROMHSTSFlag;

typedef enum {
	CDROM_HINT_INT_BITMASK = 7 << 0,
	CDROM_HINT_INT0        = 1 << 0,
	CDROM_HINT_INT1        = 1 << 1,
	CDROM_HINT_INT2        = 1 << 2,
	CDROM_HINT_BFEMPT      = 1 << 3,
	CDROM_HINT_BFWRDY      = 1 << 4
} CDROMHINTFlag;

typedef enum {
	CDROM_HCHPCTL_SMEN = 1 << 5,
	CDROM_HCHPCTL_BFWR = 1 << 6,
	CDROM_HCHPCTL_BFRD = 1 << 7
} CDROMHCHPCTLFlag;

typedef enum {
	CDROM_HCLRCTL_CLRINT_BITMASK = 7 << 0,
	CDROM_HCLRCTL_CLRINT0        = 1 << 0,
	CDROM_HCLRCTL_CLRINT1        = 1 << 1,
	CDROM_HCLRCTL_CLRINT2        = 1 << 2,
	CDROM_HCLRCTL_CLRBFEMPT      = 1 << 3,
	CDROM_HCLRCTL_CLRBFWRDY      = 1 << 4,
	CDROM_HCLRCTL_SMADPCLR       = 1 << 5,
	CDROM_HCLRCTL_CLRPRM         = 1 << 6,
	CDROM_HCLRCTL_CHPRST         = 1 << 7
} CDROMHCLRCTLFlag;

typedef enum {
	CDROM_CI_SM       = 1 << 0,
	CDROM_CI_FS       = 1 << 2,
	CDROM_CI_BITLNGTH = 1 << 4,
	CDROM_CI_EMPHASIS = 1 << 6
} CDROMCIFlag;

typedef enum {
	CDROM_ADPCTL_ADPMUTE = 1 << 0,
	CDROM_ADPCTL_CHNGATV = 1 << 5
} CDROMADPCTLFlag;

#define CDROM_HSTS      _MMIO8(IO_BASE | 0x800) // All banks
#define CDROM_RESULT    _MMIO8(IO_BASE | 0x801) // All banks
#define CDROM_RDDATA    _MMIO8(IO_BASE | 0x802) // All banks
#define CDROM_HINTMSK_R _MMIO8(IO_BASE | 0x803) // Bank 0
#define CDROM_HINTSTS   _MMIO8(IO_BASE | 0x803) // Bank 1

#define CDROM_ADDRESS   _MMIO8(IO_BASE | 0x800) // All banks
#define CDROM_COMMAND   _MMIO8(IO_BASE | 0x801) // Bank 0
#define CDROM_PARAMETER _MMIO8(IO_BASE | 0x802) // Bank 0
#define CDROM_HCHPCTL   _MMIO8(IO_BASE | 0x803) // Bank 0
#define CDROM_WRDATA    _MMIO8(IO_BASE | 0x801) // Bank 1
#define CDROM_HINTMSK_W _MMIO8(IO_BASE | 0x802) // Bank 1
#define CDROM_HCLRCTL   _MMIO8(IO_BASE | 0x803) // Bank 1
#define CDROM_CI        _MMIO8(IO_BASE | 0x801) // Bank 2
#define CDROM_ATV0      _MMIO8(IO_BASE | 0x802) // Bank 2
#define CDROM_ATV1      _MMIO8(IO_BASE | 0x803) // Bank 2
#define CDROM_ATV2      _MMIO8(IO_BASE | 0x801) // Bank 3
#define CDROM_ATV3      _MMIO8(IO_BASE | 0x802) // Bank 3
#define CDROM_ADPCTL    _MMIO8(IO_BASE | 0x803) // Bank 3

/* GPU */

typedef enum {
	GP1_STAT_PAGE_X_BITMASK      = 15 <<  0, // GP0_CMD_TEXPAGE
	GP1_STAT_PAGE_Y0             =  1 <<  4, // GP0_CMD_TEXPAGE
	GP1_STAT_BLEND_BITMASK       =  3 <<  5, // GP0_CMD_TEXPAGE
	GP1_STAT_BLEND_SEMITRANS     =  0 <<  5, // GP0_CMD_TEXPAGE
	GP1_STAT_BLEND_ADD           =  1 <<  5, // GP0_CMD_TEXPAGE
	GP1_STAT_BLEND_SUBTRACT      =  2 <<  5, // GP0_CMD_TEXPAGE
	GP1_STAT_BLEND_DIV4_ADD      =  3 <<  5, // GP0_CMD_TEXPAGE
	GP1_STAT_COLOR_BITMASK       =  3 <<  7, // GP0_CMD_TEXPAGE
	GP1_STAT_COLOR_4BPP          =  0 <<  7, // GP0_CMD_TEXPAGE
	GP1_STAT_COLOR_8BPP          =  1 <<  7, // GP0_CMD_TEXPAGE
	GP1_STAT_COLOR_16BPP         =  2 <<  7, // GP0_CMD_TEXPAGE
	GP1_STAT_DITHER              =  1 <<  9, // GP0_CMD_TEXPAGE
	GP1_STAT_UNLOCK_FB           =  1 << 10, // GP0_CMD_TEXPAGE
	GP1_STAT_SET_MASK            =  1 << 11, // GP0_CMD_FB_MASK
	GP1_STAT_USE_MASK            =  1 << 12, // GP0_CMD_FB_MASK
	GP1_STAT_DISP_FIELD_BITMASK  =  1 << 13,
	GP1_STAT_DISP_FIELD_EVEN     =  0 << 13,
	GP1_STAT_DISP_FIELD_ODD      =  1 << 13,
	GP1_STAT_PAGE_Y1             =  1 << 15, // GP0_CMD_TEXPAGE
	GP1_STAT_FB_HRES_BITMASK     =  7 << 16, // GP1_CMD_FB_MODE
	GP1_STAT_FB_VRES_BITMASK     =  1 << 19, // GP1_CMD_FB_MODE
	GP1_STAT_FB_VRES_256         =  0 << 19, // GP1_CMD_FB_MODE
	GP1_STAT_FB_VRES_512         =  1 << 19, // GP1_CMD_FB_MODE
	GP1_STAT_FB_MODE_BITMASK     =  1 << 20, // GP1_CMD_FB_MODE
	GP1_STAT_FB_MODE_NTSC        =  0 << 20, // GP1_CMD_FB_MODE
	GP1_STAT_FB_MODE_PAL         =  1 << 20, // GP1_CMD_FB_MODE
	GP1_STAT_FB_COLOR_BITMASK    =  1 << 21, // GP1_CMD_FB_MODE
	GP1_STAT_FB_COLOR_16BPP      =  0 << 21, // GP1_CMD_FB_MODE
	GP1_STAT_FB_COLOR_24BPP      =  1 << 21, // GP1_CMD_FB_MODE
	GP1_STAT_FB_INTERLACE        =  1 << 22, // GP1_CMD_FB_MODE
	GP1_STAT_DISP_BLANK          =  1 << 23, // GP1_CMD_DISP_BLANK
	GP1_STAT_IRQ                 =  1 << 24,
	GP1_STAT_DREQ                =  1 << 25,
	GP1_STAT_CMD_READY           =  1 << 26,
	GP1_STAT_READ_READY          =  1 << 27,
	GP1_STAT_WRITE_READY         =  1 << 28,
	GP1_STAT_DREQ_MODE_BITMASK   =  3 << 29, // GP1_CMD_DREQ_MODE
	GP1_STAT_DREQ_MODE_NONE      =  0 << 29, // GP1_CMD_DREQ_MODE
	GP1_STAT_DREQ_MODE_FIFO      =  1 << 29, // GP1_CMD_DREQ_MODE
	GP1_STAT_DREQ_MODE_GP0_WRITE =  2 << 29, // GP1_CMD_DREQ_MODE
	GP1_STAT_DREQ_MODE_GP0_READ  =  3 << 29, // GP1_CMD_DREQ_MODE
	GP1_STAT_DRAW_FIELD_BITMASK  =  1 << 31,
	GP1_STAT_DRAW_FIELD_EVEN     =  0 << 31,
	GP1_STAT_DRAW_FIELD_ODD      =  1 << 31
} GP1StatusFlag;

#define GPU_GP0 _MMIO32(IO_BASE | 0x810)
#define GPU_GP1 _MMIO32(IO_BASE | 0x814)

/* MDEC */

typedef enum {
	MDEC_CMD_LENGTH_BITMASK     = 0xffff <<  0, // MDEC_CMD_OP_DECODE
	MDEC_CMD_USE_CHROMA         =      1 <<  0, // MDEC_CMD_OP_SET_QUANT_TABLE
	MDEC_CMD_SIGNED             =      1 << 25, // MDEC_CMD_OP_DECODE
	MDEC_CMD_16BPP_MASK         =      1 << 26, // MDEC_CMD_OP_DECODE
	MDEC_CMD_FORMAT_BITMASK     =      3 << 27, // MDEC_CMD_OP_DECODE
	MDEC_CMD_FORMAT_4BPP        =      0 << 27, // MDEC_CMD_OP_DECODE
	MDEC_CMD_FORMAT_8BPP        =      1 << 27, // MDEC_CMD_OP_DECODE
	MDEC_CMD_FORMAT_24BPP       =      2 << 27, // MDEC_CMD_OP_DECODE
	MDEC_CMD_FORMAT_16BPP       =      3 << 27, // MDEC_CMD_OP_DECODE
	MDEC_CMD_OP_BITMASK         =      7 << 29,
	MDEC_CMD_OP_NOP             =      0 << 29,
	MDEC_CMD_OP_DECODE          =      1 << 29,
	MDEC_CMD_OP_SET_QUANT_TABLE =      2 << 29,
	MDEC_CMD_OP_SET_IDCT_TABLE  =      3 << 29
} MDECCommandFlag;

typedef enum {
	MDEC_STAT_LENGTH_BITMASK = 0xffff <<  0,
	MDEC_STAT_BLOCK_BITMASK  =      7 << 16,
	MDEC_STAT_BLOCK_Y0       =      0 << 16,
	MDEC_STAT_BLOCK_Y1       =      1 << 16,
	MDEC_STAT_BLOCK_Y2       =      2 << 16,
	MDEC_STAT_BLOCK_Y3       =      3 << 16,
	MDEC_STAT_BLOCK_CR       =      4 << 16,
	MDEC_STAT_BLOCK_CB       =      5 << 16,
	MDEC_STAT_16BPP_MASK     =      1 << 23,
	MDEC_STAT_SIGNED         =      1 << 24,
	MDEC_STAT_FORMAT_BITMASK =      3 << 25,
	MDEC_STAT_FORMAT_4BPP    =      0 << 25,
	MDEC_STAT_FORMAT_8BPP    =      1 << 25,
	MDEC_STAT_FORMAT_24BPP   =      2 << 25,
	MDEC_STAT_FORMAT_16BPP   =      3 << 25,
	MDEC_STAT_DREQ_OUT       =      1 << 27,
	MDEC_STAT_DREQ_IN        =      1 << 28,
	MDEC_STAT_BUSY           =      1 << 29,
	MDEC_STAT_DATA_FULL      =      1 << 30,
	MDEC_STAT_DATA_EMPTY     =      1 << 31
} MDECStatusFlag;

typedef enum {
	MDEC_CTRL_DMA_OUT = 1 << 29,
	MDEC_CTRL_DMA_IN  = 1 << 30,
	MDEC_CTRL_RESET   = 1 << 31
} MDECControlFlag;

#define MDEC0 _MMIO32(IO_BASE | 0x820)
#define MDEC1 _MMIO32(IO_BASE | 0x824)

/* SPU */

typedef enum {
	SPU_STAT_CDDA           = 1 <<  0,
	SPU_STAT_EXT            = 1 <<  1,
	SPU_STAT_CDDA_REVERB    = 1 <<  2,
	SPU_STAT_EXT_REVERB     = 1 <<  3,
	SPU_STAT_XFER_BITMASK   = 3 <<  4,
	SPU_STAT_XFER_NONE      = 0 <<  4,
	SPU_STAT_XFER_WRITE     = 1 <<  4,
	SPU_STAT_XFER_DMA_WRITE = 2 <<  4,
	SPU_STAT_XFER_DMA_READ  = 3 <<  4,
	SPU_STAT_IRQ            = 1 <<  6,
	SPU_STAT_DREQ           = 1 <<  7,
	SPU_STAT_WRITE_REQ      = 1 <<  8,
	SPU_STAT_READ_REQ       = 1 <<  9,
	SPU_STAT_BUSY           = 1 << 10,
	SPU_STAT_CAPTURE_BUF    = 1 << 11
} SPUStatusFlag;

typedef enum {
	SPU_CTRL_CDDA           = 1 <<  0,
	SPU_CTRL_EXT            = 1 <<  1,
	SPU_CTRL_CDDA_REVERB    = 1 <<  2,
	SPU_CTRL_EXT_REVERB     = 1 <<  3,
	SPU_CTRL_XFER_BITMASK   = 3 <<  4,
	SPU_CTRL_XFER_NONE      = 0 <<  4,
	SPU_CTRL_XFER_WRITE     = 1 <<  4,
	SPU_CTRL_XFER_DMA_WRITE = 2 <<  4,
	SPU_CTRL_XFER_DMA_READ  = 3 <<  4,
	SPU_CTRL_IRQ_ENABLE     = 1 <<  6,
	SPU_CTRL_REVERB         = 1 <<  7,
	SPU_CTRL_UNMUTE         = 1 << 14,
	SPU_CTRL_ENABLE         = 1 << 15
} SPUControlFlag;

#define SPU_CH_VOL_L(N)     _MMIO16((IO_BASE | 0xc00) + (16 * (N)))
#define SPU_CH_VOL_R(N)     _MMIO16((IO_BASE | 0xc02) + (16 * (N)))
#define SPU_CH_FREQ(N)      _MMIO16((IO_BASE | 0xc04) + (16 * (N)))
#define SPU_CH_ADDR(N)      _MMIO16((IO_BASE | 0xc06) + (16 * (N)))
#define SPU_CH_ADSR1(N)     _MMIO16((IO_BASE | 0xc08) + (16 * (N)))
#define SPU_CH_ADSR2(N)     _MMIO16((IO_BASE | 0xc0a) + (16 * (N)))
#define SPU_CH_ADSR_VOL(N)  _MMIO16((IO_BASE | 0xc0c) + (16 * (N)))
#define SPU_CH_LOOP_ADDR(N) _MMIO16((IO_BASE | 0xc0e) + (16 * (N)))

#define SPU_MASTER_VOL_L _MMIO16(IO_BASE | 0xd80)
#define SPU_MASTER_VOL_R _MMIO16(IO_BASE | 0xd82)
#define SPU_REVERB_VOL_L _MMIO16(IO_BASE | 0xd84)
#define SPU_REVERB_VOL_R _MMIO16(IO_BASE | 0xd86)
#define SPU_FLAG_ON1     _MMIO16(IO_BASE | 0xd88)
#define SPU_FLAG_ON2     _MMIO16(IO_BASE | 0xd8a)
#define SPU_FLAG_OFF1    _MMIO16(IO_BASE | 0xd8c)
#define SPU_FLAG_OFF2    _MMIO16(IO_BASE | 0xd8e)
#define SPU_FLAG_FM1     _MMIO16(IO_BASE | 0xd90)
#define SPU_FLAG_FM2     _MMIO16(IO_BASE | 0xd92)
#define SPU_FLAG_NOISE1  _MMIO16(IO_BASE | 0xd94)
#define SPU_FLAG_NOISE2  _MMIO16(IO_BASE | 0xd96)
#define SPU_FLAG_REVERB1 _MMIO16(IO_BASE | 0xd98)
#define SPU_FLAG_REVERB2 _MMIO16(IO_BASE | 0xd9a)
#define SPU_FLAG_STATUS1 _MMIO16(IO_BASE | 0xd9c)
#define SPU_FLAG_STATUS2 _MMIO16(IO_BASE | 0xd9e)

#define SPU_REVERB_ADDR _MMIO16(IO_BASE | 0xda2)
#define SPU_IRQ_ADDR    _MMIO16(IO_BASE | 0xda4)
#define SPU_ADDR        _MMIO16(IO_BASE | 0xda6)
#define SPU_DATA        _MMIO16(IO_BASE | 0xda8)
#define SPU_CTRL        _MMIO16(IO_BASE | 0xdaa)
#define SPU_DMA_CTRL    _MMIO16(IO_BASE | 0xdac)
#define SPU_STAT        _MMIO16(IO_BASE | 0xdae)

#define SPU_CDDA_VOL_L _MMIO16(IO_BASE | 0xdb0)
#define SPU_CDDA_VOL_R _MMIO16(IO_BASE | 0xdb2)
#define SPU_EXT_VOL_L  _MMIO16(IO_BASE | 0xdb4)
#define SPU_EXT_VOL_R  _MMIO16(IO_BASE | 0xdb6)
#define SPU_VOL_STAT_L _MMIO16(IO_BASE | 0xdb8)
#define SPU_VOL_STAT_R _MMIO16(IO_BASE | 0xdba)

#define SPU_REVERB_DAPF1   _MMIO16(IO_BASE | 0xdc0)
#define SPU_REVERB_DAPF2   _MMIO16(IO_BASE | 0xdc2)
#define SPU_REVERB_VIIR    _MMIO16(IO_BASE | 0xdc4)
#define SPU_REVERB_VCOMB1  _MMIO16(IO_BASE | 0xdc6)
#define SPU_REVERB_VCOMB2  _MMIO16(IO_BASE | 0xdc8)
#define SPU_REVERB_VCOMB3  _MMIO16(IO_BASE | 0xdca)
#define SPU_REVERB_VCOMB4  _MMIO16(IO_BASE | 0xdcc)
#define SPU_REVERB_VWALL   _MMIO16(IO_BASE | 0xdce)
#define SPU_REVERB_VAPF1   _MMIO16(IO_BASE | 0xdd0)
#define SPU_REVERB_VAPF2   _MMIO16(IO_BASE | 0xdd2)
#define SPU_REVERB_MLSAME  _MMIO16(IO_BASE | 0xdd4)
#define SPU_REVERB_MRSAME  _MMIO16(IO_BASE | 0xdd6)
#define SPU_REVERB_MLCOMB1 _MMIO16(IO_BASE | 0xdd8)
#define SPU_REVERB_MRCOMB1 _MMIO16(IO_BASE | 0xdda)
#define SPU_REVERB_MLCOMB2 _MMIO16(IO_BASE | 0xddc)
#define SPU_REVERB_MRCOMB2 _MMIO16(IO_BASE | 0xdde)
#define SPU_REVERB_DLSAME  _MMIO16(IO_BASE | 0xde0)
#define SPU_REVERB_DRSAME  _MMIO16(IO_BASE | 0xde2)
#define SPU_REVERB_MLDIFF  _MMIO16(IO_BASE | 0xde4)
#define SPU_REVERB_MRDIFF  _MMIO16(IO_BASE | 0xde6)
#define SPU_REVERB_MLCOMB3 _MMIO16(IO_BASE | 0xde8)
#define SPU_REVERB_MRCOMB3 _MMIO16(IO_BASE | 0xdea)
#define SPU_REVERB_MLCOMB4 _MMIO16(IO_BASE | 0xdec)
#define SPU_REVERB_MRCOMB4 _MMIO16(IO_BASE | 0xdee)
#define SPU_REVERB_DLDIFF  _MMIO16(IO_BASE | 0xdf0)
#define SPU_REVERB_DRDIFF  _MMIO16(IO_BASE | 0xdf2)
#define SPU_REVERB_MLAPF1  _MMIO16(IO_BASE | 0xdf4)
#define SPU_REVERB_MRAPF1  _MMIO16(IO_BASE | 0xdf6)
#define SPU_REVERB_MLAPF2  _MMIO16(IO_BASE | 0xdf8)
#define SPU_REVERB_MRAPF2  _MMIO16(IO_BASE | 0xdfa)
#define SPU_REVERB_VLIN    _MMIO16(IO_BASE | 0xdfc)
#define SPU_REVERB_VRIN    _MMIO16(IO_BASE | 0xdfe)

/* CW33300 CPU configuration */

typedef enum {
	CPU_BCC_LOCK           = 1 <<  0,
	CPU_BCC_INV            = 1 <<  1,
	CPU_BCC_TAG            = 1 <<  2,
	CPU_BCC_RAM            = 1 <<  3,
	CPU_BCC_DBLKSZ_BITMASK = 3 <<  4,
	CPU_BCC_DBLKSZ_2       = 0 <<  4,
	CPU_BCC_DBLKSZ_4       = 1 <<  4,
	CPU_BCC_DS             = 1 <<  7,
	CPU_BCC_IBLKSZ_BITMASK = 3 <<  8,
	CPU_BCC_IBLKSZ_2       = 0 <<  8,
	CPU_BCC_IBLKSZ_4       = 1 <<  8,
	CPU_BCC_IS0            = 1 << 10,
	CPU_BCC_IS1            = 1 << 11,
	CPU_BCC_INTP           = 1 << 12,
	CPU_BCC_RDPRI          = 1 << 13,
	CPU_BCC_NOPAD          = 1 << 14,
	CPU_BCC_BGNT           = 1 << 15,
	CPU_BCC_LDSCH          = 1 << 16,
	CPU_BCC_NOSTR          = 1 << 17
} CPUBCCFlag;

#define CPU_BCC _MMIO32(CPU_BASE | 0x130)

/*
 * ps1-bare-metal - (C) 2023-2025 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


#include <stdbool.h>
#include <stddef.h>


#define DEF(type) static inline type __attribute__((always_inline))

/* DMA tags */

DEF(uint32_t) gp0_tag(size_t length, void *next) {
	return 0
		| (((uint32_t) next   & 0xffffff) <<  0)
		| (((uint32_t) length & 0x0000ff) << 24);
}

DEF(uint32_t) gp0_endTag(size_t length) {
	return gp0_tag(length, (void *) 0xffffff);
}

/* Drawing attributes */

typedef enum {
	GP0_BLEND_BITMASK   = 3,
	GP0_BLEND_SEMITRANS = 0,
	GP0_BLEND_ADD       = 1,
	GP0_BLEND_SUBTRACT  = 2,
	GP0_BLEND_DIV4_ADD  = 3
} GP0BlendMode;

typedef enum {
	GP0_COLOR_BITMASK = 3,
	GP0_COLOR_4BPP    = 0,
	GP0_COLOR_8BPP    = 1,
	GP0_COLOR_16BPP   = 2
} GP0ColorDepth;

DEF(uint16_t) gp0_page(
	unsigned int  x,
	unsigned int  y,
	GP0BlendMode  blendMode,
	GP0ColorDepth colorDepth
) {
	return 0
		| ((x          & 15) <<  0)
		| ((y          &  1) <<  4)
		| ((blendMode  &  3) <<  5)
		| ((colorDepth &  3) <<  7)
		| ((y          &  2) << 10);
}
DEF(uint16_t) gp0_clut(unsigned int x, unsigned int y) {
	return 0
		| ((x & 0x03f) << 0)
		| ((y & 0x3ff) << 6);
}

DEF(uint32_t) gp0_xy(int x, int y) {
	return 0
		| ((x & 0xffff) <<  0)
		| ((y & 0xffff) << 16);
}
DEF(uint32_t) gp0_uv(unsigned int u, unsigned int v, uint16_t attr) {
	return 0
		| ((u    & 0x00ff) <<  0)
		| ((v    & 0x00ff) <<  8)
		| ((attr & 0xffff) << 16);
}
DEF(uint32_t) gp0_rgb(uint8_t r, uint8_t g, uint8_t b) {
	return 0
		| ((r & 0xff) <<  0)
		| ((g & 0xff) <<  8)
		| ((b & 0xff) << 16);
}

/* GP0 (drawing) commands */

typedef enum {
	GP0_CMD_MISC       = 0 << 29,
	GP0_CMD_POLYGON    = 1 << 29,
	GP0_CMD_LINE       = 2 << 29,
	GP0_CMD_RECTANGLE  = 3 << 29,
	GP0_CMD_VRAM_BLIT  = 4 << 29,
	GP0_CMD_VRAM_WRITE = 5 << 29,
	GP0_CMD_VRAM_READ  = 6 << 29,
	GP0_CMD_ATTRIBUTE  = 7 << 29
} GP0Command;

typedef enum {
	GP0_CMD_NOP         = GP0_CMD_MISC | ( 0 << 24),
	GP0_CMD_FLUSH_CACHE = GP0_CMD_MISC | ( 1 << 24),
	GP0_CMD_VRAM_FILL   = GP0_CMD_MISC | ( 2 << 24),
	GP0_CMD_NOP2        = GP0_CMD_MISC | ( 3 << 24),
	GP0_CMD_IRQ         = GP0_CMD_MISC | (31 << 24)
} GP0MiscCommand;

typedef enum {
	GP0_CMD_TEXPAGE    = GP0_CMD_ATTRIBUTE | (1 << 24),
	GP0_CMD_TEXWINDOW  = GP0_CMD_ATTRIBUTE | (2 << 24),
	GP0_CMD_FB_OFFSET1 = GP0_CMD_ATTRIBUTE | (3 << 24),
	GP0_CMD_FB_OFFSET2 = GP0_CMD_ATTRIBUTE | (4 << 24),
	GP0_CMD_FB_ORIGIN  = GP0_CMD_ATTRIBUTE | (5 << 24),
	GP0_CMD_FB_MASK    = GP0_CMD_ATTRIBUTE | (6 << 24)
} GP0AttributeCommand;

DEF(uint32_t) _gp0_polygon(
	bool quad,
	bool unshaded,
	bool gouraud,
	bool textured,
	bool blend
) {
	return GP0_CMD_POLYGON
		| ((unshaded & 1) << 24)
		| ((blend    & 1) << 25)
		| ((textured & 1) << 26)
		| ((quad     & 1) << 27)
		| ((gouraud  & 1) << 28);
}
DEF(uint32_t) gp0_triangle(bool textured, bool blend) {
	return _gp0_polygon(false, true, false, textured, blend);
}
DEF(uint32_t) gp0_shadedTriangle(bool gouraud, bool textured, bool blend) {
	return _gp0_polygon(false, false, gouraud, textured, blend);
}
DEF(uint32_t) gp0_quad(bool textured, bool blend) {
	return _gp0_polygon(true, true, false, textured, blend);
}
DEF(uint32_t) gp0_shadedQuad(bool gouraud, bool textured, bool blend) {
	return _gp0_polygon(true, false, gouraud, textured, blend);
}

DEF(uint32_t) gp0_line(bool gouraud, bool blend) {
	return GP0_CMD_LINE
		| ((blend   & 1) << 25)
		| ((gouraud & 1) << 28);
}
DEF(uint32_t) gp0_polyLine(bool gouraud, bool blend) {
	return GP0_CMD_LINE
		| ((blend   & 1) << 25)
		| (1             << 27)
		| ((gouraud & 1) << 28);
}

DEF(uint32_t) _gp0_rectangle(
	uint8_t size,
	bool    textured,
	bool    unshaded,
	bool    blend
) {
	return GP0_CMD_RECTANGLE
		| ((unshaded & 1) << 24)
		| ((blend    & 1) << 25)
		| ((textured & 1) << 26)
		| ((size     & 3) << 27);
}
DEF(uint32_t) gp0_rectangle(bool textured, bool unshaded, bool blend) {
	return _gp0_rectangle(0, textured, unshaded, blend);
}
DEF(uint32_t) gp0_rectangle1x1(bool textured, bool unshaded, bool blend) {
	return _gp0_rectangle(1, textured, unshaded, blend);
}
DEF(uint32_t) gp0_rectangle8x8(bool textured, bool unshaded, bool blend) {
	return _gp0_rectangle(2, textured, unshaded, blend);
}
DEF(uint32_t) gp0_rectangle16x16(bool textured, bool unshaded, bool blend) {
	return _gp0_rectangle(3, textured, unshaded, blend);
}

DEF(uint32_t) gp0_vramBlit(void) {
	return GP0_CMD_VRAM_BLIT;
}
DEF(uint32_t) gp0_vramWrite(void) {
	return GP0_CMD_VRAM_WRITE;
}
DEF(uint32_t) gp0_vramRead(void) {
	return GP0_CMD_VRAM_READ;
}

DEF(uint32_t) gp0_flushCache(void) {
	return GP0_CMD_FLUSH_CACHE;
}
DEF(uint32_t) gp0_vramFill(void) {
	return GP0_CMD_VRAM_FILL;
}
DEF(uint32_t) gp0_irq(void) {
	return GP0_CMD_IRQ;
}

DEF(uint32_t) gp0_texpage(uint16_t page, bool dither, bool unlockFB) {
	return GP0_CMD_TEXPAGE
		| ((page     & 0x9ff) <<  0)
		| ((dither   &     1) <<  9)
		| ((unlockFB &     1) << 10);
}
DEF(uint32_t) gp0_texwindow(
	uint8_t baseX,
	uint8_t baseY,
	uint8_t maskX,
	uint8_t maskY
) {
	return GP0_CMD_TEXWINDOW
		| ((maskX & 0x1f) <<  0)
		| ((maskY & 0x1f) <<  5)
		| ((baseX & 0x1f) << 10)
		| ((baseY & 0x1f) << 15);
}
DEF(uint32_t) gp0_fbOffset1(unsigned int x, unsigned int y) {
	return GP0_CMD_FB_OFFSET1
		| ((x & 0x3ff) <<  0)
		| ((y & 0x3ff) << 10);
}
DEF(uint32_t) gp0_fbOffset2(unsigned int x, unsigned int y) {
	return GP0_CMD_FB_OFFSET2
		| ((x & 0x3ff) <<  0)
		| ((y & 0x3ff) << 10);
}
DEF(uint32_t) gp0_fbOrigin(int x, int y) {
	return GP0_CMD_FB_ORIGIN
		| ((x & 0x7ff) <<  0)
		| ((y & 0x7ff) << 11);
}
DEF(uint32_t) gp0_fbMask(bool setMask, bool useMask) {
	return GP0_CMD_FB_MASK
		| (setMask << 0)
		| (useMask << 1);
}

/* GP1 (display control) commands */

typedef enum {
	GP1_HRES_BITMASK = (3 << 0) | (1 << 6),
	GP1_HRES_256     = 0 << 0, // Dotclock divided by 10
	GP1_HRES_320     = 1 << 0, // Dotclock divided by 8
	GP1_HRES_368     = 1 << 6, // Dotclock divided by 7
	GP1_HRES_512     = 2 << 0, // Dotclock divided by 5
	GP1_HRES_640     = 3 << 0  // Dotclock divided by 4
} GP1HorizontalRes;

typedef enum {
	GP1_VRES_BITMASK = 1,
	GP1_VRES_256     = 0,
	GP1_VRES_512     = 1
} GP1VerticalRes;

typedef enum {
	GP1_MODE_BITMASK = 1,
	GP1_MODE_NTSC    = 0,
	GP1_MODE_PAL     = 1
} GP1VideoMode;

typedef enum {
	GP1_COLOR_BITMASK = 1,
	GP1_COLOR_16BPP   = 0,
	GP1_COLOR_24BPP   = 1
} GP1ColorDepth;

typedef enum {
	GP1_DREQ_BITMASK   = 3,
	GP1_DREQ_NONE      = 0,
	GP1_DREQ_FIFO      = 1,
	GP1_DREQ_GP0_WRITE = 2,
	GP1_DREQ_GP0_READ  = 3
} GP1DMARequestMode;

typedef enum {
	GP1_VRAM_BITMASK = 1,
	GP1_VRAM_1MB     = 0,
	GP1_VRAM_2MB     = 1
} GP1VRAMSize;

typedef enum {
	GP1_CMD_RESET_GPU   =  0 << 24,
	GP1_CMD_RESET_FIFO  =  1 << 24,
	GP1_CMD_ACKNOWLEDGE =  2 << 24,
	GP1_CMD_DISP_BLANK  =  3 << 24,
	GP1_CMD_DREQ_MODE   =  4 << 24,
	GP1_CMD_FB_OFFSET   =  5 << 24,
	GP1_CMD_FB_RANGE_H  =  6 << 24,
	GP1_CMD_FB_RANGE_V  =  7 << 24,
	GP1_CMD_FB_MODE     =  8 << 24,
	GP1_CMD_VRAM_SIZE   =  9 << 24,
	GP1_CMD_GET_INFO    = 16 << 24
} GP1Command;

DEF(uint32_t) gp1_clockMultiplierH(GP1HorizontalRes horizontalRes) {
	switch (horizontalRes) {
		case GP1_HRES_256:
			return 10;
		case GP1_HRES_320:
			return 8;
		case GP1_HRES_368:
			return 7;
		case GP1_HRES_512:
			return 5;
		case GP1_HRES_640:
			return 4;
		default:
			return 0;
	}
}

DEF(uint32_t) gp1_clockDividerV(GP1VerticalRes verticalRes) {
	switch (verticalRes) {
		case GP1_VRES_256:
			return 1;
		case GP1_VRES_512:
			return 2;
		default:
			return 0;
	}
}

DEF(uint32_t) gp1_resetGPU(void) {
	return GP1_CMD_RESET_GPU;
}
DEF(uint32_t) gp1_resetFIFO(void) {
	return GP1_CMD_RESET_FIFO;
}
DEF(uint32_t) gp1_acknowledge(void) {
	return GP1_CMD_ACKNOWLEDGE;
}
DEF(uint32_t) gp1_dispBlank(bool blank) {
	return GP1_CMD_DISP_BLANK
		| (blank & 1);
}
DEF(uint32_t) gp1_dmaRequestMode(GP1DMARequestMode mode) {
	return GP1_CMD_DREQ_MODE
		| (mode & 3);
}
DEF(uint32_t) gp1_fbOffset(unsigned int x, unsigned int y) {
	return GP1_CMD_FB_OFFSET
		| ((x & 0x3ff) <<  0)
		| ((y & 0x3ff) << 10);
}
DEF(uint32_t) gp1_fbRangeH(unsigned int low, unsigned int high) {
	return GP1_CMD_FB_RANGE_H
		| ((low  & 0xfff) <<  0)
		| ((high & 0xfff) << 12);
}
DEF(uint32_t) gp1_fbRangeV(unsigned int low, unsigned int high) {
	return GP1_CMD_FB_RANGE_V
		| ((low  & 0x3ff) <<  0)
		| ((high & 0x3ff) << 10);
}
DEF(uint32_t) gp1_fbMode(
	GP1HorizontalRes horizontalRes,
	GP1VerticalRes   verticalRes,
	GP1VideoMode     videoMode,
	bool             interlace,
	GP1ColorDepth    colorDepth
) {
	return GP1_CMD_FB_MODE
		| ((horizontalRes & 0x47) << 0)
		| ((verticalRes   &    1) << 2)
		| ((videoMode     &    1) << 3)
		| ((colorDepth    &    1) << 4)
		| ((interlace     &    1) << 5);
}
DEF(uint32_t) gp1_vramSize(GP1VRAMSize size) {
	return GP1_CMD_VRAM_SIZE
		| (size & 1);
}

#undef DEF


static void setupGPU(GP1VideoMode mode, int width, int height) {
	// Set the origin of the displayed framebuffer. These "magic" values,
	// derived from the GPU's internal clocks, will center the picture on most
	// displays and upscalers.
	int x = 0x760;
	int y = (mode == GP1_MODE_PAL) ? 0xa3 : 0x88;

	// Set the resolution. The GPU provides a number of fixed horizontal (256,
	// 320, 368, 512, 640) and vertical (240-256, 480-512) resolutions to pick
	// from, which affect how fast pixels are output and thus how "stretched"
	// the framebuffer will appear.
	GP1HorizontalRes horizontalRes = GP1_HRES_320;
	GP1VerticalRes   verticalRes   = GP1_VRES_256;

	// Set the number of displayed rows and columns. These values are in GPU
	// clock units rather than pixels, thus they are dependent on the selected
	// resolution.
	int offsetX = (width  * gp1_clockMultiplierH(horizontalRes)) / 2;
	int offsetY = (height / gp1_clockDividerV(verticalRes))      / 2;

	// Hand all parameters over to the GPU by sending GP1 commands.
	GPU_GP1 = gp1_resetGPU();
	GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
	GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
	GPU_GP1 = gp1_fbMode(
		horizontalRes,
		verticalRes,
		mode,
		false,
		GP1_COLOR_16BPP
	);
}

static void waitForGP0Ready(void) {
	// Block until the GPU reports to be ready to accept commands through its
	// status register (which has the same address as GP1 but is read-only).
	while (!(GPU_GP1 & GP1_STAT_CMD_READY))
		__asm__ volatile("");
}

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

int main(int argc, const char **argv) {
	// Initialize the serial interface. The initSerialIO() function is defined
	// in src/libc/misc.c and does basically the same things we did in the
	// previous example. Afterwards, we'll be able to use puts(), printf() and
	// a few other standard I/O functions as they are declared in the libc
	// directory (with printf() being provided by a third-party library).
	//initSerialIO(115200);

	// Read the GPU's status register to check if it was left in PAL or NTSC
	// mode by the BIOS/loader.
	if ((GPU_GP1 & GP1_STAT_FB_MODE_BITMASK) == GP1_STAT_FB_MODE_PAL) {
		//puts("Using PAL mode");
		setupGPU(GP1_MODE_PAL, SCREEN_WIDTH, SCREEN_HEIGHT);
	} else {
		//puts("Using NTSC mode");
		setupGPU(GP1_MODE_NTSC, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	// Wait for the GPU to become ready, then send some GP0 commands to tell it
	// which area of the framebuffer we want to draw to and enable dithering.
	waitForGP0Ready();
	GPU_GP0 = gp0_texpage(0, true, false);
	GPU_GP0 = gp0_fbOffset1(0, 0);
	GPU_GP0 = gp0_fbOffset2(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
	GPU_GP0 = gp0_fbOrigin(0, 0);

	// Send a VRAM fill command to quickly fill our area with solid gray. Note
	// that the coordinates passed to this specific command are *not* relative
	// to the ones we've just sent to the GPU!
	waitForGP0Ready();
	GPU_GP0 = gp0_rgb(64, 64, 64) | gp0_vramFill();
	GPU_GP0 = gp0_xy(0, 0);
	GPU_GP0 = gp0_xy(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Tell the GPU to draw a Gouraud shaded triangle whose vertices are red,
	// green and blue respectively at the center of our drawing area.
	waitForGP0Ready();
	GPU_GP0 = gp0_rgb(255, 0, 0) | gp0_shadedTriangle(true, false, false);
	GPU_GP0 = gp0_xy(SCREEN_WIDTH / 3, 32);
	GPU_GP0 = gp0_rgb(0, 255, 0);
	GPU_GP0 = gp0_xy(32, SCREEN_HEIGHT - 32);
	GPU_GP0 = gp0_rgb(0, 0, 255);
	GPU_GP0 = gp0_xy(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32);

	// Send two GP1 commands to set the origin of the area we want to display
	// and switch on the display output.
	GPU_GP1 = gp1_fbOffset(0, 0);
	GPU_GP1 = gp1_dispBlank(false);

	// Continue by doing nothing.
	for (;;)
		__asm__ volatile("");

	return 0;
}
