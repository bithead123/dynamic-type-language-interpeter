#include "vm.h"

VM vm;

INTERPRET_RESULT run() {
    #define READ_BYTE() (*vm.instr_ptr++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    for (;;) {

        #ifdef DEBUG_TRACE_EXECUTION
        disasm_chunk_code(vm.chunk, (int)(vm.instr_ptr-vm.chunk->code));
        #endif

        uint8_t code;
        switch (code = READ_BYTE())
        {
        case OP_RET:
            return INTERPRET_OK;
        
        case OP_CONST:
            Value constant = READ_CONSTANT();
            print_value(constant);
            printf("\n");
            break;

        default:
            break;
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
}

INTERPRET_RESULT vm_interpret(Chunk* t) {
    vm.chunk = t;
    vm.instr_ptr = vm.chunk->code; // ptr to first instruction 
    return run();
};

void vm_init() {};
void vm_destroy() {};