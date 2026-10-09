#include <cdrom.h>
#include <interrupts.h>
#include <dma.h>
#include <hardware.h>

volatile CD_HINTSTS cdrom_current_status = CD_IRQ_S_NOIRQ;

uint8_t cdrom_command_response[16];

bool cdrom_up = false;

static void read_result(void){
    _MMIO8(CD_REG0) = CD_R0_BANK1;
    cdrom_current_status = (_MMIO8(CD_REG3) & CD_INTERRUPT_RMASK);
  
    for(size_t i = 0; i < 16; i++){
        cdrom_command_response[i] = 0;
    }

    size_t index = 0;
    while(_MMIO8(CD_REG0) & CD_RES_RRDY){
        cdrom_command_response[index] = _MMIO8(CD_REG1);
        index++;
    }
    _MMIO8(CD_REG0) = CD_R0_BANK1;
    
    _MMIO8(CD_REG3) = CD_ACK_IRQ;
    _MMIO8(CD_REG0) = CD_R0_BANK0;


}

static void cdrom_await_response(void){
    while(cdrom_current_status == CD_IRQ_S_NOIRQ){
        __asm__ volatile("");
    }
}

void cdrom_init(void){   
    if(cdrom_up){return;}

    DMA_MASTER_ARM_ON;
    enable_dma_channel(DMA_CH_CDROM);

    _MMIO8(CD_REG0) = CD_R0_BANK1;
    _MMIO8(CD_REG3) = CD_ACK_IRQ;

    _MMIO8(CD_REG0)  = CD_R0_BANK0;
    _MMIO8(CD_REG3)  = CD_HCHPCTL_INIT;
    _MMIO32(COM_DELAY) = 0x1325;
    _MMIO8(CD_REG0)  = CD_R0_BANK1;
    _MMIO8(CD_REG3)  = CD_ACK_IRQ;
    _MMIO8(CD_REG2)  = CD_ENABLE_IRQ;
    _MMIO8(CD_REG0)  = CD_R0_BANK0;

    set_interrupt_channel(I_MASK_CDROM, true);


    for (size_t i = 0; i < 2; i++)
    {       
       if(!cdrom_issue_cmd(CD_CMD_NOP,0,NULL, CD_IRQ_S_ACK,true)){return;}
    }    
    
    if(!cdrom_issue_cmd(CD_CMD_INIT,0,NULL, CD_IRQ_S_CMD_FIN,true)){return;}
    
    if(cdrom_issue_cmd(CD_CMD_DEMUTE,0,NULL, CD_IRQ_S_ACK,true)){cdrom_up=true;}
     
}

bool cdrom_issue_cmd(CD_COMMAND cmd, uint8_t argc, CD_ARGUMENT* argv, CD_HINTSTS expected_response, bool sync){
    
    CDROM_SYNC;
    cdrom_current_status = CD_IRQ_S_NOIRQ;
    _MMIO8(CD_REG0) = CD_R0_BANK0;
    if(argv){
        for(uint8_t i = 0; i < argc; i++){
            _MMIO8(CD_REG2) = argv[i];
        }
    }
    _MMIO8(CD_REG0) = CD_R0_BANK0;
    _MMIO8(CD_REG1) = cmd;

    if(sync){
        while (cdrom_current_status!=expected_response && cdrom_current_status!=CD_IRQ_S_ERR)
        {      
            cdrom_await_response();
        }  
    }

    bool ok = !(CDROM_ERR);
    cdrom_current_status = CD_IRQ_S_NOIRQ;
    return ok;
}

void cdrom_irq(void){
  
    read_result();   

}