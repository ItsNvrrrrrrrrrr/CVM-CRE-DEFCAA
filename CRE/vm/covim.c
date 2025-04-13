#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytecode.h"
#include "runtime.h"

void run_vm(const char *filename, int debug) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open bytecode file");
        exit(EXIT_FAILURE);
    }
    uint8_t magic_bytes[4];
    if (fread(magic_bytes, 1, 4, file) != 4) {
        fprintf(stderr, "Failed to read magic number from file\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    if (magic_bytes[0] != 0xFA || magic_bytes[1] != 0xAC ||
        magic_bytes[2] != 0xBE || magic_bytes[3] != 0xED) {
        fprintf(stderr, "Invalid bytecode file: incorrect magic number\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    Bytecode* bytecode = load_bytecode(filename);
    if (!bytecode) {
        fprintf(stderr, "Failed to load bytecode from %s\n", filename);
        exit(EXIT_FAILURE);
    }
    VM *vm = create_vm();
    if (!vm) {
        fprintf(stderr, "Failed to create VM\n");
        exit(EXIT_FAILURE);
    }
    execute(vm, bytecode, debug);
    free_vm(vm);
    free_bytecode(bytecode);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bytecode file> [--debug]\n", argv[0]);
        return EXIT_FAILURE;
    }
    int debug = 0;
    if (argc >= 3 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
    }
    run_vm(argv[1], debug);
    return EXIT_SUCCESS;
}