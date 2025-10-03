#include <interrupts.h>



void generic_irq_test(){


    const char msg[6] ="EVENT\0";

    char *p = malloc(6);

    if(p){
        

        for (size_t i = 0; i < 6; i++)
        {
            p[i]=msg[i];
        }
        
    }


}