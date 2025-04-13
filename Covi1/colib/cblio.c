#include <unistd.h>
#include <stdint.h>

void print(const char* text) {
    const char* p = text;
    while (*p) {
        char c = *p;
        __asm__ __volatile__ (
            "movq $1, %%rax\n\t"   // syscall number 1 = write
            "movq $1, %%rdi\n\t"   // file descriptor 1 (stdout)
            "movq %0, %%rsi\n\t"   // address of character c
            "movq $1, %%rdx\n\t"   // length = 1 byte
            "syscall\n\t"
            :
            : "r"(&c)
            : "%rax", "%rdi", "%rsi", "%rdx"
        );
        p++;
    }
}

void printnl(const char* text) {
    const char* p = text;
    while (*p) {
        char c = *p;
        __asm__ __volatile__ (
            "movq $1, %%rax\n\t"
            "movq $1, %%rdi\n\t"
            "movq %0, %%rsi\n\t"
            "movq $1, %%rdx\n\t"
            "syscall\n\t"
            :
            : "r"(&c)
            : "%rax", "%rdi", "%rsi", "%rdx"
        );
        p++;
    }
    char nl = '\n';
    __asm__ __volatile__ (
        "movq $1, %%rax\n\t"
        "movq $1, %%rdi\n\t"
        "movq %0, %%rsi\n\t"
        "movq $1, %%rdx\n\t"
        "syscall\n\t"
        :
        : "r"(&nl)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
}