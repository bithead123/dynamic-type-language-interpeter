#include "common.h"
#include "stdio.h"
#include "chunk.h"
#include "vm.h"
#include "stdlib.h"
#include "string.h"
#include "tools/hashtable.h"

void repl() {
    char line[1024];
    for(;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        vm_interpret_source(line);
    }
};


char* readFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "readFile: Can't open file with path='%s'.\n", path);
        exit(2);
    }

    fseek(f, 0L, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(fsize+1);
    if (buffer == NULL) {
        fprintf(stderr, "readFile: not enough memory for malloc buffer.\n");
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fsize, f);
    if (bytesRead < fsize) {
        fprintf(stderr, "readFile: can't read file fully.\n");
        exit(74);
    }
    
    buffer[bytesRead] = '\0';
    
    fclose(f);
    return buffer;
};

void runFile(const char* path) {
    char* source =  readFile(path);
    printf(":SOURCE=%s\n", source);
    INTERPRET_RESULT result = vm_interpret_source(source);
    free(source);

    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
    if (result == INTERPRET_COMPILE_ERROR) exit(65);
};


int main(int argc, const char* argv[]) {
    
    vm_init();

    if (argc == 1) {
        // read from stdin
        repl();
    }
    else if (argc == 2) {
        // run from file
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: vm [path]\n");
        exit(64);
    }

    vm_destroy();

    return 0;
}; 