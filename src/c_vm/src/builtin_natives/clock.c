#include "clock.h"
#include "string.h"

Value _clock(_native_call_params_) {
    _native_ok_;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
};