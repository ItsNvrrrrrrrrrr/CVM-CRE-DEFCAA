#ifndef VM_H
#define VM_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <string>

// Define magic numbers for bytecode identification
const uint32_t JAVA_MAGIC = 0xCAFEBABE;
const uint32_t CUSTOM_MAGIC = 0x00DEFCAA;

// Enum for custom opcodes
enum Opcode {
    PUSH = 0x01,
    ADD = 0x02,
    PRINT = 0x03,      // in ra giá trị (không xuống dòng)
    PRINTLN = 0x0A,    // in ra giá trị và xuống dòng
    PRINT_NO_NL = 0x1A, // in ra giá trị mà không xuống dòng

    // Control flow opcodes:
    INPUT = 0x20,      // đã có
    JMP = 0x40,        // nhảy tới địa chỉ
    JZ = 0x41,         // nhảy nếu top stack = 0
    JNZ = 0x42,        // nhảy nếu top stack != 0

    // Function calls:
    CALL = 0x43,
    RET = 0x44,

    // Mini standard lib:
    READ_INT = 0x60,
    WRITE_STR = 0x61,
    LEN = 0x62,
    STRCMP = 0x63
};

// Minimal Virtual Machine class supporting custom bytecode
class VirtualMachine {
public:
    VirtualMachine();
    void push(uint8_t value);
    void add();
    void print();            // hiện tại in ra với xuống dòng (PRINT)
    void println();          // mới: in ra với xuống dòng (PRINTLN)
    void printNoNewline();   // mới: in ra không xuống dòng (PRINT_NO_NL)
    
    VirtualMachine(const std::string& bytecodeFile);
    void execute();

private:
    std::vector<uint8_t> bytecode;
    std::stack<uint8_t> stack;
    static const size_t MAX_STACK_SIZE = 256; // ví dụ giới hạn stack

    void loadBytecode(const std::string& bytecodeFile);
    void executeJavaBytecode();
    void executeCustomBytecode();
    void handleCustomOpcode(uint8_t opcode);
    void checkStackOverflow();
    void checkStackUnderflow();
};

// Hàm chạy VM dựa trên file bytecode (dành cho cả Java và custom)
void runVM(const std::string& filename);

// Error handling functions
void throwError(const std::string& message);

#endif // VM_H