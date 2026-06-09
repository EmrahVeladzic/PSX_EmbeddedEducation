#include <cdrom.h>
#include <interrupts.h>

static volatile CD_HINTSTS current_status = CD_IRQ_S_NOIRQ;

uint8_t command_response[16];

static void read_result(void){
    _MMIO8(CD_REG0) = CD_R0_BANK1;
    current_status = (_MMIO8(CD_REG3) & CD_INTERRUPT_RMASK);
  
    for(size_t i = 0; i < 16; i++){
        command_response[i] = 0;
    }

    size_t index = 0;
    while(_MMIO8(CD_REG0) & CD_RES_RRDY){
        command_response[index] = _MMIO8(CD_REG1);
        index++;
    }
    _MMIO8(CD_REG0) = CD_R0_BANK1;
    
    _MMIO8(CD_REG3) = CD_ACK_IRQ;
    _MMIO8(CD_REG0) = CD_R0_BANK0;



   
}

static void cdrom_await_response(void){
    while(current_status == CD_IRQ_S_NOIRQ){
        __asm__ volatile("");
    }
}

void cdrom_init(void){   

    _MMIO8(CD_REG0) = CD_R0_BANK1;
    _MMIO8(CD_REG3) = CD_ACK_IRQ;


    _MMIO8(CD_REG0)  = CD_R0_BANK0;
    _MMIO8(CD_REG3)  = CD_HCHPCTL_INIT;
    _MMIO32(COM_DELAY) = 0x1325;
    _MMIO8(CD_REG0)  = CD_R0_BANK1;
    _MMIO8(CD_REG3)  = CD_ACK_IRQ;
    _MMIO8(CD_REG2)  = CD_ENABLE_IRQ;
    _MMIO8(CD_REG0)  = CD_R0_BANK0;

    set_interrupt_channel(I_MASK_CDROM, 1);

    for (size_t i = 0; i < 2; i++)
    {
        cdrom_issue_cmd(CD_CMD_NOP,0,NULL, CD_IRQ_S_ACK);
       
    }
    
    cdrom_issue_cmd(CD_CMD_INIT,0,NULL, CD_IRQ_S_CMD_FIN);
  

    cdrom_issue_cmd(CD_CMD_DEMUTE,0,NULL, CD_IRQ_S_ACK);
     
}

void cdrom_issue_cmd(CD_COMMAND cmd, uint8_t argc, CD_ARGUMENT* argv, CD_HINTSTS expected_response){
    

    current_status = CD_IRQ_S_NOIRQ;
    _MMIO8(CD_REG0) = CD_R0_BANK0;
    if(argv){
        for(uint8_t i = 0; i < argc; i++){
            _MMIO8(CD_REG2) = argv[i];
        }
    }
    _MMIO8(CD_REG0) = CD_R0_BANK0;
    _MMIO8(CD_REG1) = cmd;

    while (current_status!=expected_response && current_status!=CD_IRQ_S_ERR)
    {      
        cdrom_await_response();
    }  

}

void cdrom_irq(void){
  
    read_result(); 

    const char msg[12] ="INTERRUPT - ";
    
    char* out = malloc(16);

    if(out){
        for (size_t i = 0; i < 12; i++)
        {
            out[i]=msg[i];
        }
        out[12]=current_status+48;
        out[13]='\0';

    }


   

    free(out);
   
   
}