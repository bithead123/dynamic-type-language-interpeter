#include "common.h"
#include "stdio.h"
#include "chunk.h"
#include "vm.h"

int main() {
    
    vm_init();

    Chunk t;
    chunk_init(&t, 6);

    int ct = chunk_add_constant(&t, 25.23);
    chunk_write(&t, OP_CONST, 1);
    chunk_write(&t, ct, 1);

    int ct2 = chunk_add_constant(&t, 277.9);
    chunk_write(&t, OP_CONST, 1);
    chunk_write(&t, ct2, 1);

    chunk_write(&t, OP_DIV, 1);

    chunk_write(&t, OP_RET, 123);

    vm_interpret(&t);
    
    vm_destroy();
    chunk_destroy(&t);

    printf("end of main!\n");

    return 0;
}; 