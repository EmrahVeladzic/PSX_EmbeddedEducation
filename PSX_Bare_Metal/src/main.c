#include <stdlib.h>
#include <critical.h>

int main(){

    enter_crit_section();

    int8_t *array = malloc(48);

   

    exit_crit_section();

    const char *A = "AAAAA";
    const char *B = "BBBBB";  

    enter_crit_section();
    
    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }
  
    for (size_t i = 0; i < 48; i++)
    {
       array[i]=A[0];
    }  

    exit_crit_section();

    while (1) {
        exit_crit_section();

    }

    return 0;
}