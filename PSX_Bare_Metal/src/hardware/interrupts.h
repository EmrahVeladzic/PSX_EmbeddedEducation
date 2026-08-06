#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdlib.h>
#include <mmio.h>

#define I_MASK 0x1F801074


typedef enum i_msk{
    I_MASK_NONE = 0x000,
    I_MASK_VBLANK = 0x001,
    I_MASK_GPU = 0x002,
    I_MASK_CDROM = 0x004,
    I_MASK_DMA = 0x008,
    I_MASK_TMR0 = 0x010,
    I_MASK_TMR1 = 0x020,
    I_MASK_TMR2 = 0x040,
    I_MASK_CTRL_MEM = 0x080,
    I_MASK_SIO = 0x100,
    I_MASK_SPU = 0x200,
    I_MASK_PERIPHERAL = 0x400,
    I_MASK_ALL = 0x7FF
} INTERRUPT_MASK_CHANNEL;

void enter_crit_section(void);
void exit_crit_section(void);

void set_interrupt_channel(INTERRUPT_MASK_CHANNEL channel, bool state);

void generic_irq_test(void);


#endif