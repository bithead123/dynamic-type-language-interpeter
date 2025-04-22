#include "debug.h"

int get_code_offset(uint8_t t) {
    switch (t)
    {
    case OP_RET:
        return 1;

    case OP_NEGATE:
        return 1;
    
    case OP_CONST:
        return 2;

    default:
        return 1;
    }
}  

void disasm_chunk(Chunk* t, const char* name) {
    printf("---- Chunk ('%s' at %p) ----\n", name, t);
    for (int offset = 0; offset < t->count;) {
        offset += disasm_chunk_code(t, offset);
    }
};

int disasm_chunk_code(Chunk* t, int offset) {
    printf("%04d ", offset);
    uint8_t instr = t->code[offset];
    printf("%4d ", t->lines[offset]);
    switch (instr)
    {
    case OP_RET:
        printf("OP_RET\n");
        break;

    case OP_CONST:
        disasm_constant_instr("OP_CONST", t, offset);
        break;

    case OP_NEGATE:
        disasm_constant_instr("OP_NEGATE", t, offset);
        break;
    
    default:
        printf("Unknown opcode %d\n", instr);
        break;
    }

    return get_code_offset(instr);
};

void disasm_constant_instr(const char* name, Chunk* t, int offset) {
    uint8_t constant_index = t->code[offset+1]; // get operand 1 (index)
    printf("%-16s %4d '", name, constant_index);
    print_value(t->constants.values[constant_index]);
    printf("'\n");
};

void print_value(Value v) {
    printf("%g", v);
};