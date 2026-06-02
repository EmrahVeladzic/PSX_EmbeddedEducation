#include <stdlib.h>
#include <hardware.h>

int main(void){
    
   

    int8_t *array = malloc(48);

    const char *A = "AAAAA";
    const char *B = "BBBBB";  
    const char *C = "CCCCC";

    for (size_t i = 0; i < 48; i++){
       array[i]=A[0];
    }  

    array = realloc(array,24);

    delay_microseconds(10000000);

    int8_t *crray = malloc(8);

    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }

    for (size_t i = 0; i < 8; i++)
    {
        crray[i]=C[0];
    }


   
    cdrom_init();

    int cmd = 0;
    
    while (1) {
       __asm__ volatile("");

     
       while (cmd<10)
       {
        cdrom_issue_cmd(CD_CMD_PAUSE,0,NULL, CD_IRQ_S_CMD_FIN);
        
        cmd++;
       }
       

       

    }

    return 0;
}