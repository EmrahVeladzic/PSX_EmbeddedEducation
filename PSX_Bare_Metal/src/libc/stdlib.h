#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stdint.h>
#include <stddef.h>

void * sbrk(ptrdiff_t increment);

void * malloc(size_t size);

void * realloc(void *ptr, size_t size);

void free(void *ptr);


#endif
