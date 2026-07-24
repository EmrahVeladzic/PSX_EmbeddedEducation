#ifndef _STDIO_H_
#define _STDIO_H_


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


typedef struct sector_time
{
    uint8_t minute;
    uint8_t second;
    uint8_t frame;
    
}SECTOR_TIME;


typedef struct psx_file
{
    size_t len_bytes;
    long current_offset;
    uint32_t lba;
}FILE;


SECTOR_TIME from_lba(size_t lba);

FILE *fopen(const char*path, const char* mode);

size_t fread(void *dest, size_t size, size_t amount, FILE * fptr);

long ftell(FILE *fptr);

int fseek(FILE *fptr, long offset, int origin);

int fclose(FILE *fptr);


#endif
