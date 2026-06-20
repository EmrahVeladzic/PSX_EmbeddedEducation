#ifndef _STDIO_H_
#define _STDIO_H_


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct sector_time
{
    uint8_t minute;
    uint8_t second;
    uint8_t frame;
    
}SECTOR_TIME;


typedef struct psx_file
{
    size_t len_bytes;
    size_t current_offset;
    uint32_t lba;
}FILE;


SECTOR_TIME from_lba(size_t lba);

FILE *fopen(const char*path, const char* mode);

size_t fread(void *dest, size_t size, size_t amount, FILE * fptr);

int fclose(FILE *fptr);


#endif
