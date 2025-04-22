#include "chunk.h"

Chunk* chunk_alloc() {
    return (Chunk*)MEM_MALLOC(sizeof(Chunk));
};

void chunk_init(Chunk* t, int code_length) {
    t->code = (uint8_t*)MEM_MALLOC(sizeof(uint8_t) * code_length);
    t->lines = (int*)MEM_MALLOC(sizeof(int) * code_length);
    valueArray_init(&t->constants, VALUES_ARRAY_INIT_CAP);
    t->capacity = code_length;
    t->count = 0;
};

void chunk_write(Chunk* t, uint8_t byte, int line) {
    if (t->count+1 > t->capacity) {
        // growing array
        int old_cap = t->capacity;
        int new_cap = GROW_CAPACITY(old_cap);
        t->capacity = new_cap;
        t->code = MEM_GROW(uint8_t, t->code, old_cap, new_cap);
        t->lines = MEM_GROW(int, t->lines, old_cap, new_cap);
    }
    else {
        t->lines[t->count] = line;
        t->code[t->count] = byte;
        t->count++;
    }
};

int chunk_add_constant(Chunk *t, Value constant) {
    valueArray_write(&t->constants, constant);
    return t->constants.count-1;
};

void chunk_destroy(Chunk* t) {
    MEM_FREE(uint8_t, t->code, t->capacity);
    MEM_FREE(int, t->lines, t->capacity);
    t->count = 0;
    t->capacity = 0;
    t->code = NULL;
    valueArray_destroy(&t->constants);
};
