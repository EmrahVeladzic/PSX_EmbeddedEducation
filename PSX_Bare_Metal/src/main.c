#include <stdlib.h>
#include <hardware.h>

int main(void){

    MASK_TOGGLE32(I_MASK,I_MASK_CDROM,1);
    //Disable interrupts
    enter_crit_section();

    int8_t *array = malloc(48);

    

    const char *A = "AAAAA";
    const char *B = "BBBBB";  


    exit_crit_section();

    cdrom_init();

    //TESTING INTERRUPTS - FOR A FEW SECONDS BRRAY WILL NOT APPEAR
    for (size_t i = 0; i < 4800000; i++){
       array[i%48]=A[0];
    }  


    enter_crit_section();  

    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }


  
    

    //Enable interrupts
    exit_crit_section();

    while (1) {
       __asm__ volatile("");

    }

    return 0;
}