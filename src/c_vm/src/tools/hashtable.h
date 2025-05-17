#ifndef CVM_TOOLS_HASHTABLE_H
#define CVM_TOOLS_HASHTABLE_H

//typedef struct Hashtable Hashtable;

#include "../common.h"
#include "../object.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct Hashtable {
    int count;
    int capacity;
    Entry* entries;
} Hashtable;

void hashtable_init(Hashtable* t);

void destroy_hashtable(Hashtable* t);

bool hashtable_set(Hashtable* t, ObjString* key, Value value);

void hashtable_copy(Hashtable* from, Hashtable* to);

bool hashtable_get(Hashtable* t, ObjString* key, Value* value);

bool hashtable_delete(Hashtable* t, ObjString* key);

ObjString* hashtable_find_string(Hashtable* t, const char* chars, int length, uint32_t hash);

#endif