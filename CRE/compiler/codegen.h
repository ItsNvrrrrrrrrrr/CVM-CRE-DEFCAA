#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function to initialize the code generator
void init_codegen();

// Function to generate bytecode from the abstract syntax tree (AST)
void generate_bytecode(ASTNode *root, const char *output_file);

// Function to write the bytecode to the output file
void write_bytecode(const char *output_file);

// Function to handle errors during code generation
void codegen_error(const char *message);

#ifdef __cplusplus
}
#endif

#endif // CODEGEN_H