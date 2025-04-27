#ifndef CVM_MEMORY_H
#define CVM_MEMORY_H

#include "stdlib.h"


void* realloc_ptr(void* old_ptr, size_t old_size, size_t new_size) ;

#define MEM_MALLOC(size) malloc(size)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define MEM_GROW(type, old_ptr, old_count, new_count) \
    (type*)realloc_ptr(old_ptr, sizeof(type)*old_count, sizeof(type)*new_count)

#define MEM_FREE(type, ptr, count) \
    realloc_ptr(ptr, sizeof(type) * count, 0)

#define ALLOCATE(type, count) \
    (type*)malloc(sizeof(type) * count)

/*
    malloc
    realloc
    grow_array
    memcopy
*/

#endif