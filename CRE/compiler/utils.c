#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        handle_error("Memory allocation failed");
    }
    return ptr;
}

void safe_free(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

char *duplicate_string(const char *str) {
    if (!str) return NULL;
    char *dup = (char *)safe_malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}