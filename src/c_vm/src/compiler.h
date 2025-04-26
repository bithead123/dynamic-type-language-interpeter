#ifndef CVM_COMPILER_H
#define CVM_COMPILER_H

#include "scanner.h"
#include "vm.h"

bool compile(const char* source , Chunk* chunk);

#endif