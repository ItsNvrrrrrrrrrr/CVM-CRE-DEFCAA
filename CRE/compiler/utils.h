#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

// Error handling
void report_error(const char *message);
void *safe_malloc(size_t size);
void safe_free(void *ptr);

// Logging
void log_debug(const char *message);
void log_info(const char *message);
void log_warning(const char *message);
void log_error(const char *message);

char *duplicate_string(const char *str); // added declaration

#ifdef __cplusplus
}
#endif

#endif // UTILS_H