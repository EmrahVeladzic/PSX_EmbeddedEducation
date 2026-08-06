#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include <stddef.h>


typedef enum dma_c : uint8_t{
    DMA_CH_MDEC_IN  = 0,
    DMA_CH_MDEC_OUT = 1,
    DMA_CH_GPU      = 2,
    DMA_CH_CDROM    = 3,
    DMA_CH_SPU      = 4,
    DMA_CH_PIO      = 5,
    DMA_CH_OTC      = 6,
} DMA_CHANNEL;

typedef enum dma_p : uint32_t{
    DMA_TO_DEVICE       = (1 << 0),
    DMA_STEP_BACKWARD   = (1 << 1),
    DMA_SYNC_SLICE      = (1 << 9),
    DMA_SYNC_LIST       = (2 << 9),
    DMA_START           = (1 << 24),
    DMA_FORCE           = (1 << 28),
} DMA_CHCR_FLAGS;


#define DMA_DPCR_MASK(ch)  (0x8 << ((ch) * 4))

#define DMA_DPCR 0x1F8010F0
#define DMA_DICR 0x1F8010F4

#define DMA_MADR(ch)  (0x1F801080 + (ch) * 0x10)
#define DMA_BCR(ch)   (0x1F801084 + (ch) * 0x10)
#define DMA_CHCR(ch)  (0x1F801088 + (ch) * 0x10)

#define DMA_CHCR_BUSY(ch) ((_MMIO32(DMA_CHCR(ch))>>24)&0x1)

#define DMA_SYNC(ch) while (DMA_CHCR_BUSY(ch)){ __asm__ volatile("");}

#define DMA_DICR_RMW_MASK 0x00FF7FFF

#define DMA_IRQ_CH_ON(ch) (_MMIO32(DMA_DICR)=((_MMIO32(DMA_DICR)&DMA_DICR_RMW_MASK)|(0x1u<<((ch)+16))))
#define DMA_IRQ_CH_OFF(ch) (_MMIO32(DMA_DICR)=((_MMIO32(DMA_DICR)&DMA_DICR_RMW_MASK)&~(0x1u<<((ch)+16))))

#define DMA_ACK(ch) (_MMIO32(DMA_DICR)=((_MMIO32(DMA_DICR)&DMA_DICR_RMW_MASK)|(0x1u<<((ch)+24))))
#define DMA_PENDING() ((_MMIO32(DMA_DICR)>>24)&0x7F)

#define DMA_MASTER_ARM_ON (_MMIO32(DMA_DICR)=((_MMIO32(DMA_DICR)&DMA_DICR_RMW_MASK)|0x00800000))
#define DMA_MASTER_ARM_OFF (_MMIO32(DMA_DICR)=((_MMIO32(DMA_DICR)&DMA_DICR_RMW_MASK)&~0x00800000))

#define DMA_BUS_ERR ((_MMIO32(DMA_DICR)>>15)&0x1)

void dma_irq(void);


void enable_dma_channel(DMA_CHANNEL ch);
void disable_dma_channel(DMA_CHANNEL ch);
void start_dma_transfer(DMA_CHANNEL ch, void* address, uint32_t bcr, DMA_CHCR_FLAGS flags, bool sync);



#endif