#include "common.h"
#include "stdio.h"
#include "chunk.h"
#include "debug.h"

int main() {
    
    Chunk t;
    chunk_init(&t, 5);

    int ct = chunk_add_constant(&t, 25.23);
    chunk_write(&t, OP_CONST, 123);
    chunk_write(&t, ct, 123);

    chunk_write(&t, OP_RET, 123);

    disasm_chunk(&t, "s");
    chunk_destroy(&t);

    printf("hello 32!\n");

    return 0;
}; 