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