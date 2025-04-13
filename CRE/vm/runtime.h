#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>
#include <stddef.h>

#define STACK_SIZE 256

typedef struct {
    uintptr_t *stack;    // changed from uint32_t* to uintptr_t*
    int stack_pointer;
    // Additional fields for execution context can be added here
} VM;

typedef struct Bytecode Bytecode; // Forward declaration

VM* create_vm(void);
void free_vm(VM *vm);
void push(VM *vm, uintptr_t value);
uintptr_t pop(VM *vm);
void execute(VM *vm, Bytecode *bytecode, int debug);

#endif // RUNTIME_H