#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <sstream>       // ...added for istringstream...
#include <algorithm>     // ...added for remove_if...
#include <cctype>        // ...added for isspace...
#include "vm.h"
#include "utils.h"

// Cập nhật hàm đọc magic number với xử lý endianness
uint32_t readMagicNumber(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
         throw std::runtime_error("Failed to open file: " + filename);
    }
    uint8_t bytes[4];
    file.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    if (file.gcount() != sizeof(bytes)) {
         throw std::runtime_error("Failed to read magic number from file: " + filename);
    }
    // Chuyển đổi từ định dạng Big-endian sang host order
    uint32_t magic = (static_cast<uint32_t>(bytes[0]) << 24) |
                     (static_cast<uint32_t>(bytes[1]) << 16) |
                     (static_cast<uint32_t>(bytes[2]) << 8)  |
                     (static_cast<uint32_t>(bytes[3]));
    return magic;
}

// Hàm thực thi Java bytecode (mock)
void executeJavaBytecode() {
    std::cout << "Detected Java bytecode - forwarding to JVM..." << std::endl;
    // ...existing code or mock xử lý...
}

// Update executeCustomBytecode signature to accept a generic std::istream&
void executeCustomBytecode(std::istream& file) {
    VirtualMachine vm;
    uint8_t opcode;
    
    while (file.read(reinterpret_cast<char*>(&opcode), sizeof(opcode))) {
        switch (opcode) {
            case PUSH: {
                uint8_t value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                vm.push(value);
                break;
            }
            case ADD: {
                vm.add();
                break;
            }
            case PRINT: {       // opcode 0x03: print (with newline)
                vm.print();
                break;
            }
            case PRINTLN: {     // opcode 0x0A: println (with newline)
                vm.println();
                break;
            }
            case PRINT_NO_NL: { // opcode 0x1A: print without newline
                vm.printNoNewline();
                break;
            }
            default:
                throw std::runtime_error("Unsupported opcode: " + std::to_string(opcode));
        }
    }
}

// Updated runVM to support ASCII hex bytecode (with or without "0x" prefix)
void runVM(const std::string& filename) {
    std::vector<uint8_t> fileData = readFile(filename);
    
    // Check if fileData is entirely ASCII hex (allowing whitespace and "x" or "X")
    bool asciiHex = true;
    for (auto c : fileData) {
        if (!std::isspace(c) && !((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') ||
            (c == 'x' || c == 'X'))) {
            asciiHex = false;
            break;
        }
    }
    
    if (asciiHex && !fileData.empty()) {
        std::string content(fileData.begin(), fileData.end());
        std::istringstream iss(content);
        std::string token, hexJoined;
        while (iss >> token) {
            if (token.size() >= 2 && (token.substr(0,2) == "0x" || token.substr(0,2) == "0X"))
                token = token.substr(2);
            hexJoined += token;
        }
        if (hexJoined.size() % 2 != 0)
            throw std::runtime_error("Invalid ASCII hex bytecode: odd length");
        std::vector<uint8_t> bin;
        for (size_t i = 0; i < hexJoined.size(); i += 2) {
            std::string byteStr = hexJoined.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
            bin.push_back(byte);
        }
        fileData = bin;
    }
    
    if (fileData.size() < 4)
        throw std::runtime_error("Bytecode file too small to contain magic number");
    
    uint32_t magic = (static_cast<uint32_t>(fileData[0]) << 24) |
                     (static_cast<uint32_t>(fileData[1]) << 16) |
                     (static_cast<uint32_t>(fileData[2]) << 8)  |
                     (static_cast<uint32_t>(fileData[3]));
    
    std::istringstream iss(std::string(fileData.begin() + 4, fileData.end()), std::ios::binary);
    
    if (magic == JAVA_MAGIC)
        executeJavaBytecode();
    else if (magic == CUSTOM_MAGIC)
        executeCustomBytecode(iss);
    else
        throw std::runtime_error("Invalid magic number: " + std::to_string(magic));
}

// Định nghĩa các phương thức cho custom bytecode
VirtualMachine::VirtualMachine() {
    // ...existing code nếu cần khởi tạo...
}

void VirtualMachine::push(uint8_t value) {
    if (stack.size() >= MAX_STACK_SIZE)
        throw std::runtime_error("Stack overflow detected!");
    stack.push(value);
}

void VirtualMachine::add() {
    if (stack.size() < 2)
        throw std::runtime_error("Stack underflow detected while performing ADD!");
    uint8_t a = stack.top();
    stack.pop();
    uint8_t b = stack.top();
    stack.pop();
    uint8_t result = a + b;
    stack.push(result);
}

void VirtualMachine::print() {
    // Opcode 0x03: Print the top value without a newline.
    if (stack.empty())
        throw std::runtime_error("Stack underflow detected while performing PRINT!");
    std::cout << static_cast<char>(stack.top());
    stack.pop();
}

void VirtualMachine::println() {
    // Opcode 0x0A: Print the entire stack content in push order, then add a newline.
    if (stack.empty())
        throw std::runtime_error("Stack underflow detected while performing PRINTLN!");
    std::vector<char> temp;
    while (!stack.empty()) {
        temp.push_back(static_cast<char>(stack.top()));
        stack.pop();
    }
    std::reverse(temp.begin(), temp.end());
    for (char c : temp)
        std::cout << c;
    std::cout << std::endl;
}

void VirtualMachine::printNoNewline() {
    // Opcode 0x1A: Same as PRINT – print the top value without a newline.
    if (stack.empty())
        throw std::runtime_error("Stack underflow detected while performing PRINT (no newline)!");
    std::cout << static_cast<char>(stack.top());
    stack.pop();
}