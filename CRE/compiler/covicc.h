#ifndef COVICC_H
#define COVICC_H

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <memory>

// Opcode definitions...
// ...existing opcode definitions...
const uint8_t OP_CALL = 0x60; // new opcode for method call

class SymbolTable {
    std::unordered_map<std::string, uint8_t> variables;
    uint8_t nextVarId = 0;
    // New method mapping:
    std::unordered_map<std::string, uint8_t> methods;
    uint8_t nextMethodId = 0;
public:
    uint8_t addVariable(const std::string &name) {
        if (variables.find(name) == variables.end())
            variables[name] = nextVarId++;
        return variables[name];
    }
    bool isDefined(const std::string &name) const {
        return variables.find(name) != variables.end();
    }
    uint8_t getVarId(const std::string &name) const {
        auto it = variables.find(name);
        if (it == variables.end())
            throw std::runtime_error("Undefined variable: " + name);
        return it->second;
    }
    // New methods support:
    uint8_t addMethod(const std::string &name) {
        if (methods.find(name) == methods.end())
            methods[name] = nextMethodId++;
        return methods[name];
    }
    uint8_t getMethodId(const std::string &name) const {
        auto it = methods.find(name);
        if (it == methods.end())
            throw std::runtime_error("Undefined method: " + name);
        return it->second;
    }
};

// Minimal CompilerContext with an emit helper.
struct CompilerContext {
    SymbolTable symbols;
    std::vector<uint8_t> globalCode; // For top-level code
    std::unordered_map<std::string, std::vector<uint8_t>> methodsCode; // One bytecode vector per method
    bool inObject = false;
    std::string currentObject;
    void emit(uint8_t opcode) { globalCode.push_back(opcode); }
    void emit(uint8_t opcode, uint8_t operand) {
        globalCode.push_back(opcode);
        globalCode.push_back(operand);
    }
};

// Forward declarations (stubs)
class Lexer { /* ...existing code or stub... */ };
class Parser { /* ...existing code or stub... */ };
class CodeGen { /* ...existing code or stub... */ };

// Updated forward declarations.
std::vector<uint8_t> compileBlock(std::istream &in, CompilerContext &ctx);
std::vector<uint8_t> compileSimpleAssignment(const std::string &line, CompilerContext &ctx);
std::vector<uint8_t> compileIfStatement(std::istringstream &iss, std::istream &in, CompilerContext &ctx);
std::vector<uint8_t> compileAssignment(std::istringstream &iss, CompilerContext &ctx);
std::vector<uint8_t> compileWhileStatement(std::istringstream &iss, std::istream &in, CompilerContext &ctx);

// New: Forward declaration for method call compilation
std::vector<uint8_t> compileMethodCall(const std::string &line, CompilerContext &ctx);

// Add forward declaration for the new helper:
std::vector<uint8_t> compileMethodBlock(std::istream &in, CompilerContext &ctx);

#endif // COVICC_H
