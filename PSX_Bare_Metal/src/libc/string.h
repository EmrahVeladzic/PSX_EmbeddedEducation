#ifndef _STRING_H
#define _STRING_H
#include<stdint.h>
#include<stddef.h>

bool streq(const char *a, const char *b);

size_t split_next (char *str, char separator,char **output);

size_t strlen(const char *str);

void *memcpy(void* dest, const void* src, size_t len);

void *memmove(void* dest, const void* src, size_t len);

#endif