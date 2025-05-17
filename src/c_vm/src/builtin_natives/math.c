#include "math.h"

Value _max(_native_call_params_) {
    if (argc != 2) {
        _native_call_error("Expect 2 agrs in _max(a, b)");
    }
 
    if (IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        if (AS_NUMBER(args[0]) > AS_NUMBER(args[1])) {
            _native_return_(args[0])
        } else _native_return_(args[1])
    }

    _native_call_error("Invalid params for native, expect _max(number, number).");
};

Value _min(_native_call_params_) {
    if (argc != 2) {
        _native_call_error("Expect 2 agrs in _min(a, b)");
    }
 
    if (IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        if (AS_NUMBER(args[0]) < AS_NUMBER(args[1])) {
            _native_return_(args[0])
        } else _native_return_(args[1])
    }

    _native_call_error("Invalid params for native, expect _min(number, number).");
};