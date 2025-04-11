#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
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

// Updated executeCustomBytecode with new opcodes support
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
            case PRINT: {
                vm.print();
                break;
            }
            case PRINTLN: {
                vm.println();
                break;
            }
            case PRINT_NO_NL: {
                vm.printNoNewline();
                break;
            }
            case PUSH_VAR: {  // expects two bytes: var id and immediate value
                uint8_t varId, imm;
                file.read(reinterpret_cast<char*>(&varId), sizeof(varId));
                file.read(reinterpret_cast<char*>(&imm), sizeof(imm));
                vm.variables[static_cast<char>(varId)] = imm;
                break;
            }
            case LOAD_VAR: {  // expects one byte: var id
                uint8_t varId;
                file.read(reinterpret_cast<char*>(&varId), sizeof(varId));
                if (vm.variables.find(static_cast<char>(varId)) == vm.variables.end())
                    throw std::runtime_error("Variable not defined.");
                vm.push(vm.variables[static_cast<char>(varId)]);
                break;
            }
            case OP_INPUT: { // New case: prompt user for a variable value.
                uint8_t varId;
                file.read(reinterpret_cast<char*>(&varId), sizeof(varId));
                // Removed the extra output prompt.
                std::string userInput;
                std::getline(std::cin, userInput);
                int value = 0;
                try {
                    value = std::stoi(userInput);
                } catch(...) {
                    value = 0;
                }
                vm.variables[static_cast<char>(varId)] = static_cast<uint8_t>(value);
                break;
            }
            case JUMP_IF_ZERO: {
                uint8_t offset_high, offset_low;
                file.read(reinterpret_cast<char*>(&offset_high), sizeof(offset_high));
                file.read(reinterpret_cast<char*>(&offset_low), sizeof(offset_low));
                int offset = (offset_high << 8) | offset_low;
                if (vm.isStackEmpty())
                    throw std::runtime_error("Stack underflow for jump condition.");
                uint8_t cond = vm.topStack();
                vm.popStack();
                // If the condition is zero then skip the next 'offset' bytes.
                if (cond == 0) {
                    std::streampos pos = file.tellg();
                    file.seekg(pos + static_cast<std::streamoff>(offset));
                }
                break;
            }
            case JUMP: {
                uint8_t offset_high, offset_low;
                file.read(reinterpret_cast<char*>(&offset_high), sizeof(offset_high));
                file.read(reinterpret_cast<char*>(&offset_low), sizeof(offset_low));
                int offset = (offset_high << 8) | offset_low;
                std::streampos pos = file.tellg();
                file.seekg(pos + static_cast<std::streamoff>(offset));
                break;
            }
            case PRINT_VAR: {
                uint8_t varId;
                file.read(reinterpret_cast<char*>(&varId), sizeof(varId));
                if (vm.variables.find(static_cast<char>(varId)) == vm.variables.end())
                    throw std::runtime_error("Variable not defined for PRINT_VAR.");
                int num = vm.variables[static_cast<char>(varId)];
                // Convert number to string and print directly.
                std::cout << std::to_string(num);
                break;
            }
            case 0x50: { // OP_LOAD_STRING: read string literal
                uint8_t len;
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                std::string lit;
                for (int i = 0; i < len; i++) {
                    char ch;
                    file.read(&ch, sizeof(ch));
                    lit.push_back(ch);
                }
                vm.strBuffer = lit;
                break;
            }
            case 0x51: { // OP_TO_STRING: pop numeric value and convert to string
                if (vm.isStackEmpty())
                    throw std::runtime_error("Stack underflow in OP_TO_STRING");
                uint8_t val = vm.topStack();
                vm.popStack();
                vm.strOperand = std::to_string(val);
                break;
            }
            case 0x52: { // OP_CONCAT: concatenate the literal and converted value
                vm.strBuffer = vm.strBuffer + vm.strOperand;
                break;
            }
            case 0x53: { // OP_COMPARE: pop two numbers, push 1 if first > second, else 0
                if (vm.getStackSize() < 2)
                    throw std::runtime_error("Stack underflow in OP_COMPARE");
                uint8_t b = vm.topStack();
                vm.popStack();
                uint8_t a = vm.topStack();
                vm.popStack();
                vm.push(a > b ? 1 : 0);
                break;
            }
            case 0x1F: {
                // Handle stray opcode (0x1F) as a no-op.
                break;
            }
            default:
                throw std::runtime_error("Unsupported opcode: " + std::to_string(opcode));
        }
    }
}

// Updated runVM to support ASCII hex bytecode (with or without "0x" prefix)
void runVM(const std::string& filename) {
    std::string inputFile = filename;
    // If the input file has a .covi extension, compile it using covicc to produce DEFCAA bytecode.
    if (filename.size() >= 5 && filename.substr(filename.size()-5) == ".covi") {
        std::string outputFile = filename.substr(0, filename.size()-5) + ".cb";
        std::string command = "covicc " + filename + " -o " + outputFile;
        if (system(command.c_str()) != 0) {
            throw std::runtime_error("Compilation using covicc failed.");
        }
        inputFile = outputFile;
    }
    
    std::vector<uint8_t> fileData = readFile(inputFile);
    
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
    strBuffer = "";
    strOperand = "";
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
    // If a concatenated string is present, print it and clear both buffers.
    if (!strBuffer.empty()){
        std::cout << strBuffer;
        strBuffer.clear();
        strOperand.clear();
        return;
    }
    if (stack.empty())
        throw std::runtime_error("Stack underflow detected while performing PRINT!");
    std::cout << static_cast<char>(stack.top());
    stack.pop();
}

void VirtualMachine::println() {
    // Before printing, clear any leftover concatenation buffers.
    if (!strBuffer.empty()){
        std::cout << strBuffer;
        strBuffer.clear();
        strOperand.clear();
    }
    if (stack.empty()) {
        std::cout << std::endl;
    } else {
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
}

void VirtualMachine::printNoNewline() {
    if (!strBuffer.empty()){
        std::cout << strBuffer;
        strBuffer.clear();
        strOperand.clear();
        return;
    }
    if (stack.empty())
        throw std::runtime_error("Stack underflow detected while performing PRINT (no newline)!");
    std::cout << static_cast<char>(stack.top());
    stack.pop();
}