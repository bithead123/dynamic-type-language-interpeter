#include "object.h"
#include "stdio.h"
#include "string.h"
#include "vm.h"

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
    
    case OBJ_FUNCTION:
        ObjFunction* func = (ObjFunction*)t;
        chunk_destroy(&func->chunk);
        FREE(ObjFunction, func);
        break;

    case OBJ_NATIVE:
        FREE(ObjNative, t);
        break;

    case OBJ_CLOSURE:
        FREE(ObjClosure, t);

    default:
        break;
    }
};

#define ALLOCATE_OBJ(type, objType)  \
    allocate_obj(sizeof(type), objType)


ObjFunction* new_function() {
    ObjFunction* f = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    f->arity = 0;
    f->name = NULL;
    f->upvalue_count = 0;
    chunk_init(&f->chunk, 24);
    return f;
};


ObjString* allocate_string(char* chars, int length, uint32_t hash) {
    ObjString* s = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    s->length = length;
    s->hash = hash;
    s->chars = chars;

    // vm string collect
    hashtable_set(&vm.strings, s, NULL_VAL);

    return s;
};

uint32_t hash_string(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 167777619;
    }

    return hash;
};

ObjString* new_string(const char* chars, int length) {
    uint32_t hash = hash_string(chars, length);

    ObjString* intern_str = hashtable_find_string(&vm.strings, chars, length, hash);
    if (intern_str != NULL) return intern_str;

    return allocate_string(chars, length, hash);
};

ObjString* copy_string(const char* chars, int length) {
    uint32_t hash = hash_string(chars, length);

    ObjString* intern_str = hashtable_find_string(&vm.strings, chars, length, hash);
    if (intern_str != NULL) return intern_str;

    char* heapChars = ALLOCATE(char, length+1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocate_string(heapChars, length, hash);
};

ObjNative* new_native(NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
};

ObjClosure* new_closure(ObjFunction* function) {
    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    return closure;
};