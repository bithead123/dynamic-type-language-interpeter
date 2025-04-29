#ifndef CVM_COMPILER_H
#define CVM_COMPILER_H

//#define COMPILER_DEBUG_TRACE

#ifdef COMPILER_DEBUG_TRACE
#define COMPILER_DEBUG_LOG(msg) printf(msg)
#endif

#include "scanner.h"
#include "vm.h"

bool compile(const char* source , Chunk* chunk);

#endif