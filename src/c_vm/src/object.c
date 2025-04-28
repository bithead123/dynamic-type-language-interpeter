#include "object.h"
#include "stdio.h"
#include "string.h"


bool is_obj_type(Value v, ObjType type) {
    return IS_OBJ(v) && AS_OBJ(v)->type == type;
};

Obj* allocate_obj(size_t size, ObjType type) {
    Obj* t = malloc(size);
    t->type = type;

    t->next = vm.objects;
    vm.objects = t;

    return t;
};

void freeObj(Obj* t) {
    switch (t->type)
    {
    case OBJ_STRING:
        ObjString* str = (ObjString*)t;
        free(str->chars);
        FREE(ObjString, t);
        break;
    
    default:
        break;
    }
};

#define ALLOCATE_OBJ(type, objType)  \
    allocate_obj(sizeof(type), objType)

ObjString* allocate_string(char* chars, int length, uint32_t hash) {
    ObjString* s = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    s->length = length;
    s->hash = hash;
    s->chars = chars;
    return s;
};

ObjString* new_string(const char* chars, int length) {
    uint32_t hash = hash_string(chars, length);
    return allocate_string(chars, length, hash);
};

uint32_t hash_string(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 167777619;
    }

    return hash;
}

ObjString* copy_string(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length+1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    uint32_t hash = hash_string(chars, length);
    return allocate_string(heapChars, length, hash);
};