#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stddef.h>

#define MAGIC_NUMBER 0xFAACBEED

// Bytecode now holds the raw instruction bytes (after the 4-byte magic header)
typedef struct Bytecode {
    uint8_t *instructions; // Instruction stream (opcodes and data)
    size_t length;         // Number of bytes in the instruction stream
} Bytecode;

Bytecode* load_bytecode(const char *filename);
void free_bytecode(Bytecode *bytecode);

#endif // BYTECODE_H