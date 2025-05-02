#ifndef CVM_BUILTIN_NATIVES_H
#define CVM_BUILTIN_NATIVES_H

#include "../values.h"
#include "../object.h"

#define _native_call_params_ int argc, Value* args, bool* success

#define _native_ok_             (*success = true)
#define _native_fail_           (*success = false)
#define _native_ret_null_       return NULL_VAL;
#define _native_write_error_(msg)   (args[-1] = OBJ_VAL(copy_string(msg, sizeof(msg))))

#define _native_return_(expr) {_native_ok_; return expr;}

#define _native_call_error(msg) \
    _native_fail_; \
    _native_write_error_(msg); \
    _native_ret_null_; \

#endif