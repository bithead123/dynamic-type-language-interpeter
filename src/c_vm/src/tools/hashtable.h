#ifndef CVM_TOOLS_HASHTABLE_H
#define CVM_TOOLS_HASHTABLE_H

#include "../common.h"
#include "../values.h"
#include "../object.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Hashtable;

void init_hashtable(Hashtable* t);

void destroy_hashtable(Hashtable* t);

bool hashtable_set(Hashtable* t, ObjString* key, Value value);

void hashtable_copy(Hashtable* from, Hashtable* to);

bool hashtable_get(Hashtable* t, ObjString* key, Value* value);

bool hashtable_delete(Hashtable* t, ObjString* key);

#endif