#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bytecode* load_bytecode(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open bytecode file");
        return NULL;
    }
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    if(file_size < 4) {
        fprintf(stderr, "Bytecode file too short\n");
        fclose(file);
        return NULL;
    }
    // Skip the 4-byte magic header
    fseek(file, 4, SEEK_SET);
    size_t instructions_length = file_size - 4;
    Bytecode *bytecode = malloc(sizeof(Bytecode));
    if (!bytecode) exit(EXIT_FAILURE);
    bytecode->instructions = malloc(instructions_length);
    if (!bytecode->instructions) { free(bytecode); exit(EXIT_FAILURE); }
    if(fread(bytecode->instructions, 1, instructions_length, file) != instructions_length) {
        fprintf(stderr, "Error reading bytecode instructions\n");
        free(bytecode->instructions);
        free(bytecode);
        fclose(file);
        return NULL;
    }
    bytecode->length = instructions_length;
    fclose(file);
    return bytecode;
}

void free_bytecode(Bytecode *bytecode) {
    if (bytecode) {
        free(bytecode->instructions);
        free(bytecode);
    }
}

// Additional functions for interpreting bytecode can be added here