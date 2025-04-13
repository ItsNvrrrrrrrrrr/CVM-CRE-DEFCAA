#include "covicc.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Read the entire source file into a dynamically allocated buffer.
static char* read_source(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *buffer = (char*) safe_malloc(size + 1);
    if (fread(buffer, 1, size, file) != (size_t)size) {
        perror("Error reading source file");
        exit(EXIT_FAILURE);
    }
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char **argv) {
    if (argc != 4 || strcmp(argv[2], "-o") != 0) {
        fprintf(stderr, "Usage: %s <source.covi> -o <output.fac>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *src_file = argv[1];
    const char *out_file = argv[3];

    char *source = read_source(src_file);
    // Parse the Covi source into an AST (dummy parser implementation).
    ASTNode *ast = parse(source);
    if (!ast) {
        fprintf(stderr, "Parsing failed.\n");
        free(source);
        exit(EXIT_FAILURE);
    }
    // Generate bytecode from the AST.
    generate_bytecode(ast, out_file);
    free_ast(ast);
    free(source);
    printf("Compilation completed: %s\n", out_file);
    return EXIT_SUCCESS;
}