#include <string.h>

bool streq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a++ != *b++) return false;
    }
    return *a == *b;
}

size_t strlen(const char *str){
    size_t len = 0;

    while (str[len])
    {
        len++;
    }    

    return len;
}


size_t split_next (char *str, char separator,char **output){

    size_t len = strlen(str);
    *output = str;

    if(separator && len){

        for (size_t i = 1; i < len; i++)
        {
            if(str[i-1]==separator){
                *output = str+i;
                return len-i;
            }
        }
        
    }  

    return len;

}

void *memcpy(void* dest, const void* src, size_t len){
    if(len!=0&&dest!=src){
        uint8_t *destination = (uint8_t*)dest;
        const uint8_t *source = (uint8_t*)src;
        for (size_t i = 0; i < len; i++){
            destination[i]=source[i];
        }
    }   
    return dest;
}

void *memmove(void* dest, const void* src, size_t len){
    if(len!=0&&dest!=src){
        uint8_t *destination = (uint8_t*)dest;
        const uint8_t *source = (uint8_t*)src;
        if(dest<src){
            for (size_t i = 0; i < len; i++){
                destination[i]=source[i];
            }            
        }
        else{
             for (size_t i = len; i > 0; i--){
                destination[i-1]=source[i-1];
            }  
        }       
    }   
    return dest;
}