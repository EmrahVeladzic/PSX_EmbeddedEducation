#include <stdlib.h>


int main(){

    int8_t *array = malloc(48);

    const char *A = "AAAAA";
    const char *B = "BBBBB";  
    
    int8_t *brray = malloc(48);

    for (size_t i = 0; i < 48; i++)
    {
        brray[i]=B[0];
    }
  
    for (size_t i = 0; i < 48; i++)
    {
       array[i]=A[0];
    }  


    while (1) {
    }

    return 0;
}