#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>

std::vector<uint8_t> readFile(const std::string& filename);
bool checkStackOverflow(size_t stackSize, size_t maxStackSize);
bool checkStackUnderflow(size_t stackSize);
void printError(const std::string& message);

#endif // UTILS_H
