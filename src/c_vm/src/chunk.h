#ifndef CVM_CHUNK_H
#define CVM_CHUNK_H

#include "common.h"
#include "memory.h"
#include "values.h"

#define DEFAULT_CHUNK_CAPACITY 4

typedef enum {
    OP_RET,
    OP_CONST,
    OP_NEGATE,
    // match
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    // bools
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    // equality
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
} OP_CODE;

typedef struct {
    int count;
    int capacity;
    int* lines;
    uint8_t* code;
    ValueArray constants;
} Chunk;

/*
    init
    add
    remove
*/

extern Chunk* chunk_alloc();

extern void chunk_init(Chunk* t, int code_length);

extern void chunk_write(Chunk* t, uint8_t byte, int line);

extern int chunk_add_constant(Chunk *t, Value constant);

extern void chunk_destroy(Chunk* t);

#endif