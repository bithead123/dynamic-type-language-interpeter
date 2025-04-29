#include "vm.h"
#include "compiler.h"
#include "stdarg.h"
#include "object.h"

VM vm;

Value stack_peek(int distance) {
    return vm.stack_top[-1 - distance];
};

void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instr = vm.instr_ptr - vm.chunk->code - 1;
    int line = vm.chunk->lines[instr];
    fprintf(stderr, "[line %d] in script\n", line);
    vm_reset_stack();
};

// ------------ TOOLS
bool bool_is_falsey(Value v) {
    return IS_NULL(v) || (IS_BOOL(v) && !AS_BOOL(v));
};

bool strings_equal(ObjString* a, ObjString* b) {
    if (a->length == b->length) {
        return memcmp(a->chars, b->chars, a->length) == 0;
    }

    return false;
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) {
        return false;
    }

    switch (a.type)
    {
    case VALUE_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VALUE_NULL: return true;
    case VALUE_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    case VALUE_OBJ: return AS_OBJ(a) == AS_OBJ(b); // cmp by ptr
    
    default:
        return false;
    }
};

ObjString* strings_concat() {
    ObjString* b = AS_STRING(vm_stack_pop());
    ObjString* a = AS_STRING(vm_stack_pop());
    int len = a->length + b->length;
    char* chars = ALLOCATE(char, len + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[len] = '\0';

    ObjString* str = new_string(chars, len);
    return str;
};

INTERPRET_RESULT run() {
    
    #define READ_BYTE() (*vm.instr_ptr++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define READ_STRING() AS_STRING(READ_CONSTANT())
    #define BINARY_OP(valueType, operation) \
        do {\
            if (IS_STRING(stack_peek(0)) && IS_STRING(stack_peek(1))) { \
                vm_stack_push(OBJ_VAL(strings_concat())); \
                break; \
            } \
            \
            if (!IS_NUMBER(stack_peek(0)) || !IS_NUMBER(stack_peek(1))) { \
                runtime_error("Operands must be numbers in BinaryOp."); \
                return INTERPRET_RUNTIME_ERROR; \
            } \
            double b = AS_NUMBER(vm_stack_pop()); \
            double a = AS_NUMBER(vm_stack_pop()); \
            vm_stack_push(valueType(a operation b)); \
        } while (false) \

    for (;;) {

        #ifdef DEBUG_TRACE_EXECUTION

        // dump stack each instruction
        printf("      ");
        for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");

        disasm_chunk_code(vm.chunk, (int)(vm.instr_ptr-vm.chunk->code));
        #endif

        uint8_t code; 
        switch (code = READ_BYTE())
        {
        case OP_RET:
            //print_value(vm_stack_pop());
            //printf(" RET\n");
            return INTERPRET_OK;

        case OP_NEGATE: 
            // vm_stack_push(-vm_stack_pop()); break;
            if (!IS_NUMBER(stack_peek(0))) {
                runtime_error("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }

            vm_stack_push(NUMBER_VAL(-AS_NUMBER(vm_stack_pop())));
            break;

        case OP_ADD: BINARY_OP(NUMBER_VAL, +); break;
        case OP_SUB: BINARY_OP(NUMBER_VAL, -); break;
        case OP_MUL: BINARY_OP(NUMBER_VAL, *); break;
        case OP_DIV: BINARY_OP(NUMBER_VAL, /); break;

        case OP_CONST:
            Value constant = READ_CONSTANT();
            vm_stack_push(constant);
            break; 

        case OP_NULL: vm_stack_push(NULL_VAL); break;
        case OP_TRUE: vm_stack_push(BOOL_VAl(true)); break;
        case OP_FALSE: vm_stack_push(BOOL_VAl(false)); break;

        case OP_NOT: 
            vm_stack_push(BOOL_VAl(bool_is_falsey(vm_stack_pop()))); 
            break;

        case OP_EQUAL:
            Value a = vm_stack_pop();
            Value b = vm_stack_pop();
            vm_stack_push(BOOL_VAl(valuesEqual(a, b)));
            break;

        case OP_LESS: BINARY_OP(BOOL_VAl, <); break;
        case OP_GREATER: BINARY_OP(BOOL_VAl, >); break;

        // statements
        case OP_PRINT:
            print_value(vm_stack_pop());
            printf("\n");
            break;

        case OP_DEFINE_GLOBAL:
            //printf(":OP_DEFINE_GLOBAL\n");
            ObjString* name = READ_STRING();
            hashtable_set(&vm.globals, name, stack_peek(0));
            vm_stack_pop();
            break;

        case OP_SET_GLOBAL:
            ObjString* glob_name = READ_STRING();
            if (hashtable_set(&vm.globals, glob_name, stack_peek(0))) {
                hashtable_delete(&vm.globals, glob_name); // [delete]
                runtime_error("Undefined variable '%s'.", glob_name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;

        case OP_GET_GLOBAL:
            ObjString* get_glob_name = READ_STRING();
            Value value;
            if (!hashtable_get(&vm.globals, get_glob_name, &value)) {
                runtime_error("Undefined variable '%s'.", get_glob_name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            
            vm_stack_push(value);
            break;

        case OP_POP: vm_stack_pop(); break;

        case OP_SET_LOCAL:
            uint8_t set_slot = READ_BYTE();
            vm.stack[set_slot] = stack_peek(0);
            break;

        case OP_GET_LOCAL:
            uint8_t get_slot = READ_BYTE();
            vm_stack_push(vm.stack[get_slot]);
            break;

        default:
            break;
        }
    }

    #undef READ_STRING
    #undef READ_BYTE
    #undef READ_CONSTANT
}

INTERPRET_RESULT vm_interpret_source(const char* source) {
    
    Chunk chunk;
    chunk_init(&chunk, 4);

    if (!compile(source, &chunk)) {
        chunk_destroy(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.instr_ptr = vm.chunk->code;

    INTERPRET_RESULT result = run();

    chunk_destroy(&chunk);
    return result;
};

INTERPRET_RESULT vm_interpret(Chunk* t) {
    vm.chunk = t;
    vm.instr_ptr = vm.chunk->code; // ptr to first instruction 
    return run();
};

// --------- STACK

void vm_reset_stack() {
    vm.stack_top = vm.stack;
};

void vm_stack_push(Value v) {
    *vm.stack_top = v;
    vm.stack_top++;
};

Value vm_stack_pop() {
    vm.stack_top--;
    return *vm.stack_top;
};

// --------- VM

void vm_init() {
    vm_reset_stack();
    vm.objects = NULL;
    hashtable_init(&vm.strings);
    hashtable_init(&vm.globals);
    //vm.strings = ALLOCATE(Hashtable, 1);
};


void vm_destroy() {
    Obj* t = vm.objects;
    while (t != NULL) {
        Obj* next = t->next;
        freeObj(t);
        t = next;
    }

    destroy_hashtable(&vm.strings);
    destroy_hashtable(&vm.globals);
    //FREE(Hashtable, vm.strings);
};
