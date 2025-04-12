#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>

// Function to read a binary file and return its contents as a vector of bytes
std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return {};
    }

    // Read the file contents into a vector
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

// Function to check for stack overflow
bool checkStackOverflow(size_t stackSize, size_t maxStackSize) {
    if (stackSize >= maxStackSize) {
        std::cerr << "Error: Stack overflow detected!" << std::endl;
        return true;
    }
    return false;
}

// Function to check for stack underflow
bool checkStackUnderflow(size_t stackSize) {
    if (stackSize == 0) {
        std::cerr << "Error: Stack underflow detected!" << std::endl;
        return true;
    }
    return false;
}

// Function to print error messages
void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}