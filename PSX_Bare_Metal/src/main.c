#include <stdlib.h>

int main(){

	uint8_t *chararray = malloc(10000);
   
    const char *hello = "HelloWorld";
    size_t hello_len = 10; 

    for (size_t i = 0; i < 10000; i++) {
        chararray[i] = hello[i % hello_len];
    }

	free(chararray);


	uint8_t *chararrayw = malloc(10000);
   
    const char *hellow = "HelloHELLO";

    for (size_t i = 0; i < 10000; i++) {
        chararrayw[i] = hellow[i % hello_len];
    }


    while (1) {
    }

    return 0;
}