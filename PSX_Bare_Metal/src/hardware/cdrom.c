#include <cdrom.h>
#include <interrupts.h>

void cdrom_init(void){
    

    _MMIO8(CD_REG0)=CD_R0_BANK1;
    _MMIO8(CD_REG3)=CD_ACK_IRQ;
    _MMIO8(CD_REG2)=CD_ENABLE_IRQ;
    _MMIO8(CD_REG0)=CD_R0_BANK0;
    _MMIO8(CD_REG3)=CD_HCHPCTL_INIT;   
    _MMIO32(COM_DELAY)=0x1325;   
    

    set_interrupt_channel(I_MASK_CDROM,1);  

    for (size_t i = 0; i < 2; i++)
    {
        cdrom_issue_cmd(CD_CMD_NOP,0,NULL);
    }

    cdrom_issue_cmd(CD_CMD_INIT,0,NULL);

}

void cdrom_issue_cmd(CD_COMMAND cmd, uint8_t argc, CD_ARGUMENT* argv){
      
    _MMIO8(CD_REG0)=CD_R0_BANK0;

    for (uint8_t i = 0; i < argc; i++)
    {
        _MMIO8(CD_REG2)=argv[i];
    }    

    _MMIO8(CD_REG0)=CD_R0_BANK0;

    _MMIO8(CD_REG1)=cmd; 



}


void cdrom_irq(void){

    generic_irq_test();
  
    _MMIO8(CD_REG0)=CD_R0_BANK1;
    _MMIO8(CD_REG3)=CD_CLS_P_FIFO|CD_ACK_IRQ;



}