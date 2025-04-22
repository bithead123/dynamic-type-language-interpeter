#include "common.h"
#include "stdio.h"
#include "chunk.h"
#include "vm.h"

int main() {
    
    vm_init();

    Chunk t;
    chunk_init(&t, 5);

    int ct = chunk_add_constant(&t, 25.23);
    chunk_write(&t, OP_CONST, 123);
    chunk_write(&t, ct, 123);

    vm_interpret(&t);

    chunk_write(&t, OP_RET, 123);

    chunk_destroy(&t);

    printf("end of main!\n");

    return 0;
}; 