#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "utils.h"

// A very basic (dummy) parser that supports two patterns:
// 1) printnl("...") – if found, extracts the literal between quotes.
// 2) srsl( )       – if no printnl is found, then uses a default literal.
ASTNode *parse(const char *source) {
    const char *start = strstr(source, "printnl(\"");
    if (start) {
        start += strlen("printnl(\"");
        const char *end = strchr(start, '"');
        if (!end) {
            fprintf(stderr, "Error: unterminated string literal in printnl.\n");
            exit(EXIT_FAILURE);
        }
        int len = end - start;
        char *literalText = (char *)safe_malloc(len + 1);
        strncpy(literalText, start, len);
        literalText[len] = '\0';
    
        // Create literal node holding the extracted string.
        ASTNode *lit = malloc(sizeof(ASTNode));
        if (!lit) exit(EXIT_FAILURE);
        lit->type = AST_NODE_LITERAL;
        lit->data.literal.value = literalText;
    
        // Create call node for printnl.
        ASTNode *callPrintnl = malloc(sizeof(ASTNode));
        if (!callPrintnl) exit(EXIT_FAILURE);
        callPrintnl->type = AST_NODE_CALL;
        callPrintnl->data.call.function_name = "printnl";
    
        // Build function body block: literal then call.
        ASTNode **stmts = malloc(2 * sizeof(ASTNode*));
        if (!stmts) exit(EXIT_FAILURE);
        stmts[0] = lit;
        stmts[1] = callPrintnl;
    
        ASTNode *body = malloc(sizeof(ASTNode));
        if (!body) exit(EXIT_FAILURE);
        body->type = AST_NODE_BLOCK;
        body->data.block.statements = stmts;
        body->data.block.count = 2;
    
        // Create function node for main.
        ASTNode *func = malloc(sizeof(ASTNode));
        if (!func) exit(EXIT_FAILURE);
        func->type = AST_NODE_FUNCTION;
        func->data.function.name = "main";
        func->data.function.body = body;
    
        // Top-level block containing just the main function.
        ASTNode **topStmts = malloc(1 * sizeof(ASTNode*));
        if (!topStmts) exit(EXIT_FAILURE);
        topStmts[0] = func;
        ASTNode *root = malloc(sizeof(ASTNode));
        if (!root) exit(EXIT_FAILURE);
        root->type = AST_NODE_BLOCK;
        root->data.block.statements = topStmts;
        root->data.block.count = 1;
    
        return root;
    } else {
        // No printnl("...") found; check for "srsl(".
        const char *srsl_start = strstr(source, "srsl(");
        if (!srsl_start) {
            fprintf(stderr, "Error: no printnl or srsl found in source.\n");
            exit(EXIT_FAILURE);
        }
        // Build a call node for srsl without a literal.
        ASTNode *callSrsl = malloc(sizeof(ASTNode));
        if (!callSrsl) exit(EXIT_FAILURE);
        callSrsl->type = AST_NODE_CALL;
        callSrsl->data.call.function_name = "srsl";
    
        // Build function body block with just the call.
        ASTNode **stmts = malloc(1 * sizeof(ASTNode*));
        if (!stmts) exit(EXIT_FAILURE);
        stmts[0] = callSrsl;
    
        ASTNode *body = malloc(sizeof(ASTNode));
        if (!body) exit(EXIT_FAILURE);
        body->type = AST_NODE_BLOCK;
        body->data.block.statements = stmts;
        body->data.block.count = 1;
    
        // Create function node for main.
        ASTNode *func = malloc(sizeof(ASTNode));
        if (!func) exit(EXIT_FAILURE);
        func->type = AST_NODE_FUNCTION;
        func->data.function.name = "main";
        func->data.function.body = body;
    
        // Top-level block containing just the main function.
        ASTNode **topStmts = malloc(1 * sizeof(ASTNode*));
        if (!topStmts) exit(EXIT_FAILURE);
        topStmts[0] = func;
        ASTNode *root = malloc(sizeof(ASTNode));
        if (!root) exit(EXIT_FAILURE);
        root->type = AST_NODE_BLOCK;
        root->data.block.statements = topStmts;
        root->data.block.count = 1;
    
        return root;
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;
    int i;
    switch (node->type) {
        case AST_NODE_BLOCK:
            for (i = 0; i < node->data.block.count; i++) {
                free_ast(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            free(node);
            break;
        case AST_NODE_IMPORT:
            free(node);
            break;
        case AST_NODE_FUNCTION:
            free_ast(node->data.function.body);
            free(node);
            break;
        case AST_NODE_LITERAL:
            free(node->data.literal.value);
            free(node);
            break;
        case AST_NODE_CALL:
            free(node);
            break;
        case AST_NODE_SYSCALL:
            free(node);
            break;
        case AST_NODE_RETURN:
            free(node);
            break;
        default:
            free(node);
            break;
    }
}