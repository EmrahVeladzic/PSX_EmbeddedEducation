#include <stdlib.h>
#include <stdio.h>
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

    delay_microseconds(5000000);

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


    FILE *file = fopen("ASSETS/LEAVES.WL","rb");

    fseek(file,2,SEEK_SET);

    uint16_t sample_rate;

    fread(&sample_rate,sizeof(uint16_t),1,file);

    uint32_t block_count;

    fread(&block_count,sizeof(uint32_t),1,file);

    void *data = malloc((size_t)(block_count*16));

    fread(data,16,block_count,file);

    fclose(file);


    load_audio(data,block_count,sample_rate,0);

    free(data);


    SPU_KEY_ON(0);


    while (1) {
        __asm__ volatile("");


    }

    return 0;
}