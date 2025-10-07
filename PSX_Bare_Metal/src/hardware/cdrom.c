#include <cdrom.h>

void cdrom_init(void){

    volatile uint8_t* reg0 = _ADDR8(CD_REG0);
    *reg0 &= CD_R0_BANK_WMASK;
    *reg0 |= CD_R0_BANK1;    
    volatile uint8_t* reg3 = _ADDR8(CD_REG3);
    *reg3 |= CD_ACK_IRQ;
    *reg0 &= CD_R0_BANK_WMASK;
    _MMIO32(COM_DELAY)=0x1325;
    


}