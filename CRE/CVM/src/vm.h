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

// Define magic numbers for bytecode identification.
const uint32_t JAVA_MAGIC = 0xCAFEBABE;
const uint32_t CUSTOM_MAGIC = 0x00DEFCAA;

// Enum for custom opcodes.
enum Opcode {
    PUSH = 0x01,
    ADD = 0x02,
    PRINT = 0x03,
    PRINTLN = 0x0A,
    PRINT_NO_NL = 0x1A,

    // Variables and jump opcodes.
    PUSH_VAR = 0x11,
    LOAD_VAR = 0x12,
    JUMP_IF_ZERO = 0x05,
    JUMP = 0x06,
    OP_INPUT = 0x20,   // For input scanning.
    PRINT_VAR = 0x2A,  // Print numeric variable value.

    // String processing opcodes.
    OP_LOAD_STRING = 0x50, // Load a literal string.
    OP_TO_STRING   = 0x51, // Convert number to string.
    OP_CONCAT      = 0x52, // Concatenate two strings.
    OP_COMPARE     = 0x53  // Compare: if first > second, push 1; else push 0.
};

// Minimal Virtual Machine class supporting custom bytecode.
class VirtualMachine {
public:
    VirtualMachine();
    void push(uint8_t value);
    void add();
    void print();            // PRINT: output without newline.
    void println();          // PRINTLN: output then newline.
    void printNoNewline();   // PRINT without newline.

    VirtualMachine(const std::string& bytecodeFile);
    void execute();

    // Variable table mapping variable name (string) to its value.
    std::unordered_map<std::string, uint8_t> variables;
    
    // Accessors for the execution stack.
    bool isStackEmpty() const { return stack.empty(); }
    uint8_t topStack() const { return stack.top(); }
    void popStack() { stack.pop(); }
    size_t getStackSize() const { return stack.size(); }

    // For handling string concatenation opcodes.
    std::string strBuffer;
    std::string strOperand;

private:
    std::vector<uint8_t> bytecode;
    std::stack<uint8_t> stack;
    static const size_t MAX_STACK_SIZE = 256; // Example stack limit.

    // ...existing helper functions for bytecode loading and execution...
    void loadBytecode(const std::string& bytecodeFile);
    void executeJavaBytecode();
    void executeCustomBytecode();
    void handleCustomOpcode(uint8_t opcode);
    void checkStackOverflow();
    void checkStackUnderflow();
};

// Runs the VM based on a bytecode file; handles both Java and custom bytecode.
void runVM(const std::string& filename);

// Utility function to throw standardized error messages.
void throwError(const std::string& message);

#endif // VM_H