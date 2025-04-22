#ifndef CVM_H
#define CVM_H

#include "chunk.h"
#include "debug.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} INTERPRET_RESULT;

typedef struct {
    Chunk* chunk;
    uint8_t* instr_ptr;
} VM;

extern void vm_init();
extern void vm_destroy();
extern INTERPRET_RESULT vm_interpret(Chunk* t);

#endif