#include <interrupts.h>
#include <hardware.h>
#include <string.h>

static INTERRUPT_MASK_CHANNEL FORMER_MASK = I_MASK_NONE;

void enter_crit_section(void){
    if(FORMER_MASK!=I_MASK_NONE){return;}
    FORMER_MASK = _MMIO32(I_MASK);
    MASK_TOGGLE32(I_MASK,I_MASK_ALL,false);
}
void exit_crit_section(void){
    MASK_TOGGLE32(I_MASK,FORMER_MASK,true);
    FORMER_MASK = I_MASK_NONE;
}

void set_interrupt_channel(INTERRUPT_MASK_CHANNEL channel, bool state){
    state = (state>0)? 1:0;
    if(FORMER_MASK==I_MASK_NONE){
        MASK_TOGGLE32(I_MASK,channel,state);
    }
    else{
        if (state)
        {
           FORMER_MASK|=channel;
        }
        else{
            FORMER_MASK&=~channel;
        }        
    }
}



char *ram_debug(const char *msg){
   
    char *p = malloc(strlen(msg));

    if(p){

        memcpy(p,msg,strlen(msg));

    }

    return p;
    
}

void generic_irq_test(void){

    
    ram_debug("GENERIC_IRQ");
    
}
