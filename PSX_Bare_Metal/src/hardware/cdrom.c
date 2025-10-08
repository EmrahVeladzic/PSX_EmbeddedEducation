#include <cdrom.h>

void cdrom_init(void){

    volatile uint8_t* reg0 = _ADDR8(CD_REG0);
    *reg0 &= CD_R0_BANK_WMASK;
    *reg0 |= CD_R0_BANK1;   
    volatile uint8_t* reg2 = _ADDR8(CD_REG2);
    *reg2|= CD_XINTSTS;    
    volatile uint8_t* reg3 = _ADDR8(CD_REG3);
    *reg3 |= CD_ACK_IRQ;
    *reg0 &= CD_R0_BANK_WMASK;
    _MMIO32(COM_DELAY)=0x1325;
    *reg0 &= CD_R0_BANK_WMASK;
    volatile uint8_t* reg1 = _ADDR8(CD_REG1);
    *reg1 = CD_CMD_NOP;


}

void cdrom_irq(void){

    uint8_t * msg =0;

    msg[0] = 'C';
    msg[1] = 'D';
    msg[2] = '-';
    msg[3] = 'R';
    msg[4] = 'O';
    msg[5] = 'M';
}