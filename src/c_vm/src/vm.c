#include "vm.h"
#include "compiler/compiler.h"

VM vm;

INTERPRET_RESULT run() {
    #define READ_BYTE() (*vm.instr_ptr++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(operation) \
        do {\
            double b = vm_stack_pop(); \
            double a = vm_stack_pop(); \
            vm_stack_push(a operation b); \
        } while (false) \

    for (;;) {

        #ifdef DEBUG_TRACE_EXECUTION

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
            print_value(vm_stack_pop());
            printf(" ret\n");
            return INTERPRET_OK;

        case OP_NEGATE: vm_stack_push(-vm_stack_pop()); break;
        case OP_ADD: BINARY_OP(+); break;
        case OP_SUB: BINARY_OP(-); break;
        case OP_MUL: BINARY_OP(*); break;
        case OP_DIV: BINARY_OP(/); break;

        case OP_CONST:
            Value constant = READ_CONSTANT();
            vm_stack_push(constant);
            break; 

        default:
            break;
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
}

INTERPRET_RESULT vm_interpret_source(const char* source) {
    compile(source);
    return INTERPRET_OK;
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
};

void vm_destroy() {};
