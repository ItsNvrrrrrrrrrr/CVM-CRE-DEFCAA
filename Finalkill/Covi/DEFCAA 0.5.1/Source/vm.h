#ifndef VM_H
#define VM_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Define magic numbers for bytecode identification
const uint32_t JAVA_MAGIC = 0xCAFEBABE;
const uint32_t CUSTOM_MAGIC = 0x00DEFCAA;

// Enum for custom opcodes
enum Opcode {
    PUSH = 0x01,
    ADD = 0x02,
    PRINT = 0x03,
    PRINTLN = 0x0A,
    PRINT_NO_NL = 0x1A,

    // Updated opcodes for variables and jumps:
    PUSH_VAR = 0x11,
    LOAD_VAR = 0x12,
    JUMP_IF_ZERO = 0x05,
    JUMP = 0x06,
    OP_INPUT = 0x20,  // New opcode for input scanning
    PRINT_VAR = 0x2A, // Print variable (numeric) value

    // New opcodes for string concatenation support:
    OP_LOAD_STRING = 0x50, // Load a literal string
    OP_TO_STRING   = 0x51, // Convert numeric value to string
    OP_CONCAT      = 0x52, // Concatenate the two strings
    OP_COMPARE     = 0x53  // New: Compare opcode (if first > second, push 1; else push 0)
};

// Minimal Virtual Machine class supporting custom bytecode
class VirtualMachine {
public:
    VirtualMachine();
    void push(uint8_t value);
    void add();
    void print();            // PRINT: no newline.
    void println();          // PRINTLN: prints content then newline.
    void printNoNewline();   // PRINT without newline.
    
    VirtualMachine(const std::string& bytecodeFile);
    void execute();
    
    // Variable table (variable name -> value)
    std::unordered_map<char, uint8_t> variables;
    
    // Public accessors for the stack.
    bool isStackEmpty() const { return stack.empty(); }
    uint8_t topStack() const { return stack.top(); }
    void popStack() { stack.pop(); }
    // New: get current stack size
    size_t getStackSize() const { return stack.size(); }
    
    // For new concatenation opcodes.
    std::string strBuffer;
    std::string strOperand;
    
private:
    std::vector<uint8_t> bytecode;
    std::stack<uint8_t> stack;
    static const size_t MAX_STACK_SIZE = 256; // example stack limit

    void loadBytecode(const std::string& bytecodeFile);
    void executeJavaBytecode();
    void executeCustomBytecode();
    void handleCustomOpcode(uint8_t opcode);
    void checkStackOverflow();
    void checkStackUnderflow();
};

// Run VM based on a bytecode file (for both Java and custom)
void runVM(const std::string& filename);

// Error handling functions.
void throwError(const std::string& message);

#endif // VM_H