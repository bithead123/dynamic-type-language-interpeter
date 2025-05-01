#include "clock.h"

Value clock_(int argc, Value* args) {
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
};