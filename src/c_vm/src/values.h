#ifndef CVM_VALUES_H
#define CVM_VALUES_H

typedef double Value;

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

#endif