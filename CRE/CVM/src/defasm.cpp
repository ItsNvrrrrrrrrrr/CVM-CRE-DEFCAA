#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

// Predefined opcode values (matching those in vm.h)
enum Opcode {
    OP_PUSH      = 0x01,
    OP_PRINT     = 0x03,
    OP_PRINTLN   = 0x0A,
    OP_JMP       = 0x40,
    OP_JZ        = 0x41,
    OP_JNZ       = 0x42,
    OP_CALL      = 0x43,
    OP_RET       = 0x44,
    OP_INPUT     = 0x20,
    OP_READ_INT  = 0x60,
    OP_WRITE_STR = 0x61,
    OP_LEN       = 0x62,
    OP_STRCMP    = 0x63
};

void assemble(const std::string &inputFile, const std::string &outputFile) {
    std::ifstream fin(inputFile);
    if (!fin)
        throw std::runtime_error("Failed to open input assembly file.");

    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout)
        throw std::runtime_error("Failed to open output bytecode file.");

    // Write custom magic number (0x00DEFCAA) in big-endian.
    uint32_t magic = 0x00DEFCAA;
    fout.put((magic >> 24) & 0xFF);
    fout.put((magic >> 16) & 0xFF);
    fout.put((magic >> 8) & 0xFF);
    fout.put(magic & 0xFF);

    std::string line;
    while (std::getline(fin, line)) {
        // Skip empty lines and comments (lines starting with ';' for example)
        if(line.empty() || line[0] == ';')
            continue;
        std::istringstream iss(line);
        std::string instr;
        iss >> instr;
        if (instr == "PUSH") {
            // Expect: PUSH 'H' or PUSH 72
            std::string operand;
            iss >> operand;
            char value;
            if (operand.front() == '\'' && operand.back() == '\'')
                value = operand[1]; // lấy ký tự giữa dấu nháy đơn
            else
                value = static_cast<char>(std::stoi(operand));
            fout.put(OP_PUSH);
            fout.put(value);
        }
        else if (instr == "PRINT") {
            fout.put(OP_PRINT);
        }
        else if (instr == "PRINTLN") {
            fout.put(OP_PRINTLN);
        }
        // Control flow, function calls, and others could be implemented similarly:
        else if (instr == "GOTO") {   // alias for JMP
            fout.put(OP_JMP);
            // For now, write a dummy 2-byte address (address resolution not implemented)
            fout.put(0x00);
            fout.put(0x00);
        }
        else if (instr == "CALL") {
            fout.put(OP_CALL);
            // Dummy address
            fout.put(0x00);
            fout.put(0x00);
        }
        else if (instr == "RET") {
            fout.put(OP_RET);
        }
        // Add more instructions as needed (INPUT, SUB, etc.)
    }
    fin.close();
    fout.close();
    std::cout << "Assembly complete. Output file: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: defasm <input.asm> <output.cb>" << std::endl;
        return 1;
    }
    try {
        assemble(argv[1], argv[2]);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
