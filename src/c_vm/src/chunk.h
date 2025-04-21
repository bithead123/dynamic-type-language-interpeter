#ifndef CVM_CHUNK_H
#define CVM_CHUNK_H

#include "common.h"
#include "memory.h"

typedef enum {
    OP_RET,
} OP_CODE;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
} Chunk;

/*
    init
    add
    remove
*/

Chunk* chunk_alloc() {
    return (Chunk*)MEM_MALLOC(sizeof(Chunk));
};

void chunk_init(Chunk* t, int code_length) {
    t->code = (uint8_t*)MEM_MALLOC(sizeof(uint8_t) * code_length);
    t->capacity = code_length;
    t->count = 0;
};

void chunk_write(Chunk* t, uint8_t byte) {
    if (t->count+1 > t->capacity) {
        // growing array
        int old_cap = t->capacity;
        int new_cap = GROW_CAPACITY(old_cap);
        uint8_t* new_ptr = MEM_GROW(uint8_t, t->code, old_cap, new_cap);
        t->code = new_ptr;
        t->capacity = new_cap;
    }
    else {
        t->code[t->count++] = byte;
    }
};

void chunk_destroy(Chunk* t) {
    MEM_FREE(uint8_t, t->code, t->capacity);
    t->count = 0;
    t->capacity = 0;
    t->code = NULL;
};


#endif