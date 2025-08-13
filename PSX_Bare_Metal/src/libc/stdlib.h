#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stdint.h>
#include <stddef.h>

void *malloc(size_t size);

void *realloc(void *ptr, size_t size);

void free(void *ptr);

void *memcpy(void* dest, const void* src, size_t len);

void *memmove(void* dest, const void* src, size_t len);

#endif
