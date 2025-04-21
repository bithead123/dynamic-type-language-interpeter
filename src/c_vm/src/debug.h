#ifndef CVM_DEBUG_H
#define CVM_DEBUG_H

#include "common.h"
#include "chunk.h"

void disasm_chunk(Chunk* t, const char* name) {
    printf("---- Chunk ('%s' at %p) ----\n", name, t);
    for (int offset = 0; offset < t->count; offset++) {
        disasm_chunk_code(t, offset);
    }
};

void disasm_chunk_code(Chunk* t, int offset) {
    printf("%04d ", offset);
    uint8_t instr = t->code[offset];
    switch (instr)
    {
    case OP_RET:
        printf("OP_RET\n");
        break;
    
    default:
        printf("Unknown opcode %d\n", instr);
        break;
    }
};

#endif