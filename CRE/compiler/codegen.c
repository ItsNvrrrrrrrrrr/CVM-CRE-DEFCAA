#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>  // For strcasecmp
#include "codegen.h"
#include "parser.h"
#include "utils.h"

#define OP_IMPORT     0x02
#define OP_CALL       0x03   // (For non built-in calls; unused for built-ins)
#define OP_SYSCALL    0x20
#define OP_HALT       0xFF
#define OP_LOAD_STRING 0x40  // New opcode for loading string literal
#define OP_CALL_FUNC   0x41  // New opcode for calling built-in function
#define OP_PRINT      0x50  // for built-in print (no newline)
#define OP_PRINTNL    0x51  // for built-in printnl (with newline)

static FILE *output_fp;

// Simple static list to avoid re-importing modules.
#define MAX_IMPORTED 10
static char *imported_modules[MAX_IMPORTED];
static int imported_count = 0;

static int already_imported(const char *module) {
    for (int i = 0; i < imported_count; i++) {
        if (strcmp(imported_modules[i], module) == 0) return 1;
    }
    return 0;
}

static void add_imported(const char *module) {
    if (imported_count < MAX_IMPORTED) {
        imported_modules[imported_count++] = duplicate_string(module);
    }
}

static void emit_header(FILE* out) {
    // Emit magic number in big-endian order: FA AC BE ED
    fputc(0xFA, out);
    fputc(0xAC, out);
    fputc(0xBE, out);
    fputc(0xED, out);
}

static void emit_import(FILE* out, const char* module) {
    // Avoid re-importing
    if (!already_imported(module)) {
        fputc(OP_IMPORT, out);
        uint8_t len = (uint8_t)strlen(module);
        fputc(len, out);
        fwrite(module, 1, len, out);
        add_imported(module);
    }
}

static void emit_load_string(FILE* out, const char* str) {
    fputc(OP_LOAD_STRING, out);
    uint8_t len = (uint8_t) strlen(str); // ensure correct length is used
    fputc(len, out);
    fwrite(str, 1, len, out);
}

static void emit_call_func(FILE* out, const char* func) {
    fputc(OP_CALL_FUNC, out);
    uint8_t len = (uint8_t)strlen(func);
    fputc(len, out);
    fwrite(func, 1, len, out);
}

static void emit_call(FILE* out, const char* func) {
    fputc(OP_CALL, out);
    uint8_t len = (uint8_t)strlen(func);
    fputc(len, out);
    fwrite(func, 1, len, out);
}

static void emit_syscall(FILE* out, const char* name, const char* arg_str, int arg_int) {
    fputc(OP_SYSCALL, out);
    if (strcasecmp(name, "write") == 0) {
        // Ensure we emit correct syscall ID 0x30 before string data.
        fputc(0x30, out);
        uint8_t len = (uint8_t)strlen(arg_str);
        fputc(len, out);
        fwrite(arg_str, 1, len, out);
        fputc((arg_int >> 24) & 0xFF, out);
        fputc((arg_int >> 16) & 0xFF, out);
        fputc((arg_int >> 8) & 0xFF, out);
        fputc(arg_int & 0xFF, out);
    } else if (strcasecmp(name, "nextLine") == 0) {
        fputc(0x31, out);
    }
}

static void emit_halt(FILE* out) {
    fputc(OP_HALT, out);
}

static void generate_bytecode_recursive(ASTNode *node, FILE* out) {
    if (!node) return;
    int i;
    switch (node->type) {
        case AST_NODE_BLOCK:
            for (i = 0; i < node->data.block.count; i++) {
                generate_bytecode_recursive(node->data.block.statements[i], out);
            }
            break;
        case AST_NODE_IMPORT:
            emit_import(out, node->data.import.module);
            break;
        case AST_NODE_FUNCTION:
            generate_bytecode_recursive(node->data.function.body, out);
            break;
        case AST_NODE_LITERAL:
            emit_load_string(out, node->data.literal.value);
            break;
        case AST_NODE_CALL:
            if (strcasecmp(node->data.call.function_name, "print") == 0) {
                fputc(OP_PRINT, out);
            } else if (strcasecmp(node->data.call.function_name, "printnl") == 0) {
                fputc(OP_PRINTNL, out);
            } else {
                // For other function calls, use generic OP_CALL.
                emit_call(out, node->data.call.function_name);
            }
            break;
        case AST_NODE_SYSCALL:
            if (strcasecmp(node->data.syscall.name, "write") == 0) {
                emit_syscall(out, "write", node->data.syscall.arg1, node->data.syscall.arg2);
            } else if (strcasecmp(node->data.syscall.name, "nextline") == 0) {
                emit_syscall(out, "nextLine", "", 0);
            }
            break;
        case AST_NODE_RETURN:
            // Optionally emit return opcode if needed.
            break;
        default:
            break;
    }
}

void generate_bytecode(ASTNode *root, const char *output_file) {
    output_fp = fopen(output_file, "wb");
    if (!output_fp) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    emit_header(output_fp);
    generate_bytecode_recursive(root, output_fp);
    fputc(OP_HALT, output_fp);
    fclose(output_fp);
}