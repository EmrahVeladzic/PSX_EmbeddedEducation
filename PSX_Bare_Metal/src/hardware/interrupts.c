#include <interrupts.h>

static INTERRUPT_MASK_CHANNEL FORMER_MASK = I_MASK_ALL;

void enter_crit_section(void){
    FORMER_MASK = _MMIO16(I_MASK);
    MASK_TOGGLE32(I_MASK,I_MASK_ALL,0);
}
void exit_crit_section(void){
    MASK_TOGGLE32(I_MASK,FORMER_MASK,1);
}


void generic_irq_test(void){

    static int index = 0;

    const char msg[10] ="INTERRUPT\0";

    char *p = malloc(10);

    if(p){
        

        for (size_t i = 0; i < 10; i++)
        {
            p[i]=msg[index];
        }
        index++;
        index %= 9;
    }

    free(p);

}