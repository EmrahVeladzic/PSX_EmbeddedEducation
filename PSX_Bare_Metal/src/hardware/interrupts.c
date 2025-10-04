#include <interrupts.h>

void generic_irq_test(){


    const char msg[10] ="INTERRUPT\0";

    char *p = malloc(10);

    if(p){
        

        for (size_t i = 0; i < 10; i++)
        {
            p[i]=msg[i];
        }
        
    }


}