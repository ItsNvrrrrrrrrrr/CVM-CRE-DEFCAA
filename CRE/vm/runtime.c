#include <stdio.h>   // Added: required for FILE, snprintf, fopen, fclose, printf, fwrite, stdout
#include <stdlib.h>
#include <string.h>
#include "runtime.h"
#include "bytecode.h"
#include <unistd.h> // for write()

// Extern declarations for built-in printing functions defined in cblio.c
extern void print(const char *text);
extern void printnl(const char *text);

// Updated opcode definitions for built-in calls:
#define OP_PRINT         0x50  // new: call built-in print (no newline)
#define OP_PRINTNL       0x51  // new: call built-in printnl (with newline)
#define OP_IMPORT        0x02
#define OP_CALL          0x03
#define OP_SYSCALL       0x20  // New syscall opcode
#define OP_HALT          0xFF
#define OP_LOAD_STRING   0x40  // new

// Global variable to hold imported module bytecode (for simple simulation)
static Bytecode *imported_module = NULL;

// Updated import_module: load module file from a fixed directory silently.
static void import_module(const char *module) {
    // Construct full module file path.
    char filepath[256];
    // For example, assume modules are in "/workspaces/CVM-CRE-DEFCAA/Covi1/test/"
    snprintf(filepath, sizeof(filepath), "/workspaces/CVM-CRE-DEFCAA/Covi1/test/%s", module);
    
    // Try to open the file without printing error.
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        // Simulate an empty module if file not found.
        Bytecode *mod = malloc(sizeof(Bytecode));
        if (!mod) exit(EXIT_FAILURE);
        mod->instructions = NULL;
        mod->length = 0;
        imported_module = mod;
        return;
    }
    fclose(f);
    // File exists, load its bytecode.
    Bytecode *mod = load_bytecode(filepath);
    if (!mod) {
        // Should not happen, but simulate empty module.
        mod = malloc(sizeof(Bytecode));
        if (!mod) exit(EXIT_FAILURE);
        mod->instructions = NULL;
        mod->length = 0;
    }
    imported_module = mod;
}

// Updated call_function: now check for "printnl" and "srsl"
static void call_function(const char *func) {
    // Built-in calls: printnl and srsl
    if (strcmp(func, "printnl") == 0 || strcmp(func, "srsl") == 0) {
        if (imported_module && imported_module->length > 0) {
            VM *temp_vm = create_vm();
            execute(temp_vm, imported_module, 0);
            free_vm(temp_vm);
        }
    } else {
        // For other functions, try to load the function module from a fixed directory.
        char path[256];
        // For example, assume function modules are stored in "/workspaces/CVM-CRE-DEFCAA/Covi1/colib/"
        snprintf(path, sizeof(path), "/workspaces/CVM-CRE-DEFCAA/Covi1/colib/%s.col", func);
        Bytecode *module = load_bytecode(path);
        if (module && module->length > 0) {
            VM *temp_vm = create_vm();
            execute(temp_vm, module, 0);
            free_vm(temp_vm);
        } else {
            fprintf(stderr, "Failed to load function module: %s\n", func);
            exit(EXIT_FAILURE);
        }
        if (module) free_bytecode(module);
    }
}

VM* create_vm(void) {
    VM* vm = malloc(sizeof(VM));
    if (!vm) exit(EXIT_FAILURE);
    vm->stack = malloc(STACK_SIZE * sizeof(uintptr_t)); // use uintptr_t size
    if (!vm->stack) { free(vm); exit(EXIT_FAILURE); }
    vm->stack_pointer = 0;
    return vm;
}

void push(VM* vm, uintptr_t value) {
    if (vm->stack_pointer >= STACK_SIZE) {
        exit(EXIT_FAILURE);
    }
    vm->stack[vm->stack_pointer++] = value;
}

uintptr_t pop(VM* vm) {
    if (vm->stack_pointer <= 0) {
        exit(EXIT_FAILURE);
    }
    return vm->stack[--vm->stack_pointer];
}

static void push_ptr(VM* vm, void *ptr) {
    // Push pointer value into vm->stack (cast to uint32_t).
    push(vm, (uintptr_t)ptr);
}

static void *pop_ptr(VM* vm) {
    return (void *) pop(vm);
}

void execute(VM* vm, Bytecode* bytecode, int debug) {
    int pc = 0;
    while (pc < (int)bytecode->length) {
        uint8_t opcode = bytecode->instructions[pc++];
        if (debug) {
            printf("[DEBUG] Opcode: 0x%02X at pc=%d\n", opcode, pc - 1);
        }
        switch (opcode) {
            case OP_LOAD_STRING: {
                if (pc >= (int)bytecode->length) exit(EXIT_FAILURE);
                uint8_t len = bytecode->instructions[pc++];
                if (pc + len > (int)bytecode->length) exit(EXIT_FAILURE);
                char *str = malloc(len + 1);
                if (!str) exit(EXIT_FAILURE);
                memcpy(str, &bytecode->instructions[pc], len);
                str[len] = '\0';
                push_ptr(vm, str);
                pc += len;
                break;
            }
            case OP_PRINT: {
                // Pop string from stack and call built-in print() from cblio.c.
                char *arg = pop_ptr(vm);
                print(arg);
                free(arg);
                break;
            }
            case OP_PRINTNL: {
                // Pop string from stack and call built-in printnl() from cblio.c.
                char *arg = pop_ptr(vm);
                printnl(arg);
                free(arg);
                break;
            }
            case OP_IMPORT: {
                if (pc >= (int)bytecode->length) {
                    exit(EXIT_FAILURE);
                }
                uint8_t len = bytecode->instructions[pc++];
                if (pc + len > (int)bytecode->length) {
                    exit(EXIT_FAILURE);
                }
                char *module = malloc(len + 1);
                if (!module) exit(EXIT_FAILURE);
                for (int i = 0; i < len; i++) {
                    module[i] = bytecode->instructions[pc + i];
                }
                module[len] = '\0';
                // Call import_module to load and link the module.
                import_module(module);
                free(module);
                pc += len;
                break;
            }
            case OP_CALL: {
                if (pc >= (int)bytecode->length) exit(EXIT_FAILURE);
                uint8_t len = bytecode->instructions[pc++];
                if (pc + len > (int)bytecode->length) exit(EXIT_FAILURE);
                char *func = malloc(len + 1);
                if (!func) exit(EXIT_FAILURE);
                memcpy(func, &bytecode->instructions[pc], len);
                func[len] = '\0';
                pc += len;
                // Here we call call_function() for imported module handling (unchanged)
                call_function(func);
                free(func);
                break;
            }
            case OP_SYSCALL: {
                if (pc >= (int)bytecode->length) exit(EXIT_FAILURE);
                uint8_t sys_id = bytecode->instructions[pc++];
                switch (sys_id) {
                    case 0x30: { // write syscall
                        if (pc >= (int)bytecode->length) exit(EXIT_FAILURE);
                        uint8_t str_len = bytecode->instructions[pc++];
                        if (pc + str_len > (int)bytecode->length) exit(EXIT_FAILURE);
                        char *buffer = malloc(str_len + 1);
                        if (!buffer) exit(EXIT_FAILURE);
                        memcpy(buffer, &bytecode->instructions[pc], str_len);
                        buffer[str_len] = '\0';
                        pc += str_len;
                        if (pc + 4 > (int)bytecode->length) exit(EXIT_FAILURE);
                        int arg_int = (bytecode->instructions[pc] << 24) |
                                      (bytecode->instructions[pc+1] << 16) |
                                      (bytecode->instructions[pc+2] << 8) |
                                      (bytecode->instructions[pc+3]);
                        pc += 4;
                        // Write the string using write() syscall.
                        write(1, buffer, str_len);
                        free(buffer);
                        break;
                    }
                    case 0x31: { // nextLine syscall
                        write(1, "\n", 1);
                        break;
                    }
                    default:
                        exit(EXIT_FAILURE);
                }
                break;
            }
            case OP_HALT:
                return;
            default:
                exit(EXIT_FAILURE);
        }
    }
}

void free_vm(VM* vm) {
    if (vm) {
        free(vm->stack);
        free(vm);
    }
}