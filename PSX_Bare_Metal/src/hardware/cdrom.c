#include <cdrom.h>
#include <interrupts.h>

static QueuedCDROMCommand *command_queue[CD_QUEUE_SIZE];
static uint8_t queue_next_index = 0;

uint8_t command_response[16];
volatile bool cdrom_initialized = false;

static void read_result(void){
    _MMIO8(CD_REG0) = CD_R0_BANK0;
    while(_MMIO8(CD_REG0) & CD_RES_RRDY){
        volatile uint8_t discard = _MMIO8(CD_REG1);
        (void)discard;
    }
    _MMIO8(CD_REG0) = CD_R0_BANK1;
    _MMIO8(CD_REG3) = CD_ACK_IRQ;
    _MMIO8(CD_REG0) = CD_R0_BANK0;
}

static void cdrom_wait_ack(void){
    while(!CDROM_RDY){ __asm__ volatile(""); }
    read_result();
}

void cdrom_init(void){

    cdrom_initialized = false;

    
    for (size_t i = 0; i < CD_QUEUE_SIZE; i++)
    {
        command_queue[i]=NULL;
    }
    

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
        cdrom_issue_cmd(CD_CMD_NOP,0,NULL);
       
    }


    
    cdrom_issue_cmd(CD_CMD_INIT,0,NULL);
  

    CDROM_SYNC

    cdrom_initialized=true;

}

void cdrom_issue_cmd(CD_COMMAND cmd, uint8_t argc, CD_ARGUMENT* argv){
      

    if(CDROM_RDY){

        _MMIO8(CD_REG0)=CD_R0_BANK0;

        if(argv){

            for (uint8_t i = 0; i < argc; i++)
            {
                _MMIO8(CD_REG2)=argv[i];
            }   
        
        }

        _MMIO8(CD_REG0)=CD_R0_BANK0;

        _MMIO8(CD_REG1)=cmd; 
        cdrom_wait_ack();

    }
    else{

        uint8_t start = queue_next_index;
        uint8_t end   = (queue_next_index + CD_QUEUE_SIZE - 1) % CD_QUEUE_SIZE;

        uint8_t i = start;

        do{
  
            if(!command_queue[i]){

                QueuedCDROMCommand * new_cmd = (QueuedCDROMCommand*) malloc(sizeof(QueuedCDROMCommand));

                if(new_cmd){

                    new_cmd->cmd=cmd;
                    new_cmd->argc=argc;                                     
                    new_cmd->argv = (CD_ARGUMENT*) malloc(argc*sizeof(CD_ARGUMENT));
                    if(new_cmd->argv && argv){
                        memcpy(new_cmd->argv,argv,argc * sizeof(CD_ARGUMENT));
                    }
                    else if(!new_cmd->argv && argc){
                        free(new_cmd);
                        break;
                    }
                    command_queue[i]=new_cmd;

                }               

            }

            i=((i+1)%CD_QUEUE_SIZE);

        }while (i!=((end+1)%CD_QUEUE_SIZE));
        
    }

}


void cdrom_irq(void){

    generic_irq_test();

    read_result();

    QueuedCDROMCommand *next_cmd = command_queue[queue_next_index];
    if(next_cmd){

        CDROM_SYNC
        

        cdrom_issue_cmd(next_cmd->cmd,next_cmd->argc,next_cmd->argv);

        free(next_cmd->argv);
        free(next_cmd);
        command_queue[queue_next_index]=NULL;
        
        queue_next_index = ((queue_next_index+1)%CD_QUEUE_SIZE);
       
    }

}