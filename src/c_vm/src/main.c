#include "common.h"
#include "stdio.h"
#include "chunk.h"
#include "debug.h"

int main() {
    
    Chunk t;
    chunk_init(&t, 5);
    chunk_write(&t, 0);
    chunk_write(&t, 2);
    chunk_write(&t, 3);
    disasm_chunk(&t, "s");
    chunk_destroy(&t);


    printf("hello 32!\n");

    return 0;
}; 