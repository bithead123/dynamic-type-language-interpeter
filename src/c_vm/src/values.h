#ifndef CVM_VALUES_H
#define CVM_VALUES_H

#include "common.h"
#include "memory.h"

typedef double Value;

#define VALUES_ARRAY_INIT_CAP 4

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

ValueArray* valueArray_alloc();

void valueArray_init(ValueArray* t, int array_length);

void valueArray_write(ValueArray* t, Value value);

void valueArray_destroy(ValueArray* t);

#endif