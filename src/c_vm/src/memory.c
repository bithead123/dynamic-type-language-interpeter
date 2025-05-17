#include "memory.h"

void* realloc_ptr(void* old_ptr, size_t old_size, size_t new_size) {
    if (old_ptr == NULL) {
        exit(1);
        return NULL;
    }

    if (new_size == 0) {
        free(old_ptr);
        return NULL;
    }

    void* new_ptr = realloc(old_ptr, new_size);
    if (new_ptr == NULL) {
        exit(1);
        return NULL;
    }

    return new_ptr;
};
