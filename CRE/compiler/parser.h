#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AST_NODE_BLOCK,
    AST_NODE_IMPORT,
    AST_NODE_FUNCTION,
    AST_NODE_LITERAL,   // <-- new: for string literals
    AST_NODE_CALL,      // CALL_FUNC for built-in calls like "print" or "printnl"
    AST_NODE_SYSCALL,  // <-- new for @SysCall annotation
    AST_NODE_RETURN
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Block: holds an array of statements
        struct {
            struct ASTNode **statements;
            int count;
        } block;
        // Import statement: e.g., import port.col
        struct {
            char *module;
        } import;
        // Function declaration: e.g., public void main(){ ... }
        struct {
            char *name;
            struct ASTNode *body; // function body (block node)
        } function;
        // Literal: e.g., a string literal
        struct {
            char *value;
        } literal;
        // Function call: e.g., port.srsl()
        struct {
            char *function_name;
        } call;
        // System Call: e.g., @SysCall write(s, len)
        struct {
            char *name;     // "write" or "nextLine"
            char *arg1;     // For "write": the string argument (if any)
            int arg2;       // For "write": the integer argument
        } syscall;
        // Return statement (if needed)
        struct {
            int returnValue;
        } returnNode;
    } data;
} ASTNode;

ASTNode *parse(const char *source);
void free_ast(ASTNode *node);

#ifdef __cplusplus
}
#endif

#endif // PARSER_H