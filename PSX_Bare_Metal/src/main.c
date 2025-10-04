#include <stdlib.h>
#include <critical.h>

int main(){

    //Disable interrupts
    enter_crit_section();

    int8_t *array = malloc(48);

    

    const char *A = "AAAAA";
    const char *B = "BBBBB";  

    //Malloc test, crray is and should be NULL
    int8_t *crray = malloc(0xFFFFFF);
    if(crray){
        exit_crit_section();
    }

    
    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }
  
    for (size_t i = 0; i < 48; i++)
    {
       array[i]=A[0];
    }  

    //Enable interrupts
    exit_crit_section();

    while (1) {
       

    }

    return 0;
}