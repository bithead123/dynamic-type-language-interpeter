#ifndef CVM_MEMORY_H
#define CVM_MEMORY_H

#include "stdlib.h"


void* realloc_ptr(void* old_ptr, size_t old_size, size_t new_size) {
    if (old_ptr == NULL) {
        exit(1);
        return;
    }

    if (new_size == 0) {
        free(old_ptr);
        return NULL;
    }

    void* new_ptr = realloc(old_ptr, new_size);
    if (new_ptr == NULL) {
        exit(1);
        return;
    }

    return new_ptr;
};

#define MEM_MALLOC(size) malloc(size)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define MEM_GROW(type, old_ptr, old_count, new_count) \
    (type*)realloc_ptr(old_ptr, sizeof(type)*old_count, sizeof(type)*new_count)

#define MEM_FREE(type, ptr, count) \
    realloc_ptr(ptr, sizeof(type) * count, 0)

/*
    malloc
    realloc
    grow_array
    memcopy
*/

#endif