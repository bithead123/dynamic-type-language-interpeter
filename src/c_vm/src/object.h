#ifndef CVM_OBJECT_H
#define CVM_OBJECT_H

#include "values.h"
#include "common.h"
#include "memory.h"

typedef enum ObjType {
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
};

typedef struct {
    Obj obj;
    int length;
    char* chars;
} ObjString;


ObjString* copy_string(const char* chars, int length);
bool is_obj_type(Value v, ObjType type);

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) (is_obj_type(value, OBJ_STRING))

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

#endif