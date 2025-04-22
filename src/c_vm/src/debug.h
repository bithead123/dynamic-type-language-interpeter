#ifndef CVM_DEBUG_H
#define CVM_DEBUG_H

#include "common.h"
#include "chunk.h"

extern int get_code_offset(uint8_t t);

extern void disasm_chunk(Chunk* t, const char* name) ;

extern int disasm_chunk_code(Chunk* t, int offset);

extern void disasm_constant_instr(const char* name, Chunk* t, int offset);

extern void print_value(Value v);

#endif