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

ValueArray* valueArray_alloc() {
    return (ValueArray*)MEM_MALLOC(sizeof(ValueArray));
};

void valueArray_init(ValueArray* t, int array_length) {
    t->values = (Value*)MEM_MALLOC(sizeof(Value) * array_length);
    t->capacity = array_length;
    t->count = 0;
};

void valueArray_write(ValueArray* t, Value value) {
    if (t->count+1 > t->capacity) {
        // growing array
        int old_cap = t->capacity;
        int new_cap = GROW_CAPACITY(old_cap);
        Value* new_ptr = MEM_GROW(Value, t->values, old_cap, new_cap);
        t->values = new_ptr;
        t->capacity = new_cap;
    }
    else {
        t->values[t->count++] = value;
    }
};

void valueArray_destroy(ValueArray* t) {
    MEM_FREE(Value, t->values, t->capacity);
    t->count = 0;
    t->capacity = 0;
    t->values = NULL;
};



#endif