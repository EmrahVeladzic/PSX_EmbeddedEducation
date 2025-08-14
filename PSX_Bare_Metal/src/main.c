#include <stdlib.h>

int main(){

    int8_t *array = malloc(100);

    const char *hello = "HelloWorld";
    const char *HELLOU = "HELLO";

    for (size_t i = 0; i < 100; i++)
    {
       array[i]=hello[i%10];
    }     
  
    realloc(array,10);
    
    int8_t *brray = malloc(50);



    for (size_t i = 0; i < 50; i++)
    {
        brray[i]=HELLOU[i%5];
    }



    while (1) {
    }

    return 0;
}