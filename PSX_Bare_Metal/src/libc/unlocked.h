#ifndef UNLOCKED_H
#define UNLOCKED_H

#include <stddef.h>

void *_realloc_internal(void *ptr, size_t size);

void *_malloc_internal(size_t size);

void _free_internal(void *ptr);


#endif