#include <stdlib.h>
#include <hardware.h>

int main(void){

    cdrom_init();
     
    int8_t *array = malloc(48);

    const char *A = "AAAAA";
    const char *B = "BBBBB";  

    for (size_t i = 0; i < 48; i++){
       array[i]=A[0];
    }  

    delay_microseconds(10000000);

    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }

    while (1) {
       __asm__ volatile("");

    }

    return 0;
}