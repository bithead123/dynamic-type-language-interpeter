#ifndef CVM_H
#define CVM_H

#include "chunk.h"
#include "debug.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} INTERPRET_RESULT;

#define VM_STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* instr_ptr;
    // --- stack ----
    Value stack[VM_STACK_MAX];
    Value* stack_top;
} VM;

void vm_init();
void vm_destroy();
INTERPRET_RESULT vm_interpret_source(const char* source);

void vm_stack_push(Value v);
Value vm_stack_pop();

#endif