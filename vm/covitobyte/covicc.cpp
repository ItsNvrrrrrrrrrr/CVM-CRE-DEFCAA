#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
using namespace std;

// Forward declaration for compileBlock so that it is visible to later helper functions.
std::vector<uint8_t> compileBlock(std::istream &in);

// Predefined opcodes (with new opcodes for string concatenation)
const uint8_t OP_PUSH      = 0x01;   // immediate value
const uint8_t OP_PRINT     = 0x03;   // print without newline
const uint8_t OP_PRINTLN   = 0x0A;   // print then newline
const uint8_t OP_PUSH_VAR  = 0x11;   // variable assignment
const uint8_t OP_LOAD_VAR  = 0x12;   // load variable onto stack
const uint8_t OP_JUMP_IF_ZERO = 0x05; // conditional jump
const uint8_t OP_JUMP         = 0x06; // unconditional jump
const uint8_t OP_INPUT    = 0x20;     // input scanning
const uint8_t PRINT_VAR   = 0x2A;     // print variable

// New opcodes for string concatenation support:
const uint8_t OP_LOAD_STRING = 0x50;
const uint8_t OP_TO_STRING   = 0x51;
const uint8_t OP_CONCAT      = 0x52;   // concatenate two strings
const uint8_t OP_COMPARE     = 0x53;   // compare: push 1 if first > second, else 0

// Custom magic number for DEFCAA bytecode
const uint32_t CUSTOM_MAGIC = 0x00DEFCAA;

// Helper function: trim whitespace.
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if(start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// New helper: parse assignment statement.
std::vector<uint8_t> compileAssignment(std::istringstream &iss) {
    std::vector<uint8_t> code;
    std::string var, eq, value;
    iss >> var >> eq >> value;
    if(eq != "=")
        throw std::runtime_error("Expected '=' in assignment");
    int val = std::stoi(value);
    code.push_back(OP_PUSH_VAR);
    code.push_back(static_cast<uint8_t>(var[0]));
    code.push_back(static_cast<uint8_t>(val));
    return code;
}

// New helper: parse if statement.
std::vector<uint8_t> compileIfStatement(std::istringstream &iss, std::istream &in) {
    std::vector<uint8_t> code;
    std::string leftVar, comp, rightVal;
    iss >> leftVar >> comp >> rightVal;
    // Remove surrounding parentheses from rightVal.
    while(!rightVal.empty() && (rightVal.front()=='(' || rightVal.back()==')'))
        rightVal = rightVal.substr(1, rightVal.size()-2);
    int constant = 0;
    if(comp=="<") {
        constant = std::stoi(rightVal) - 1;
    } else if(comp==">") {
        constant = std::stoi(rightVal);
    } else {
        throw std::runtime_error("Unsupported comparison operator: " + comp);
    }
    code.push_back(OP_LOAD_VAR);
    code.push_back(static_cast<uint8_t>(leftVar[0]));
    code.push_back(OP_PUSH);
    code.push_back(static_cast<uint8_t>(constant));
    code.push_back(OP_COMPARE);
    code.push_back(OP_JUMP_IF_ZERO);
    code.push_back(0x00);
    code.push_back(0x00);
    // Compile inner block.
    std::vector<uint8_t> inner = compileBlock(in);
    code.insert(code.end(), inner.begin(), inner.end());
    // Append jump at end of block.
    code.push_back(OP_JUMP);
    size_t jumpPos = code.size();
    code.push_back(0x00);
    code.push_back(0x00);
    // Back-patch jump offset (for demonstration, we simply set it zero).
    code[jumpPos] = 0;
    code[jumpPos+1] = 0;
    return code;
}

// New helper: parse while statement.
std::vector<uint8_t> compileWhileStatement(std::istringstream &iss, std::istream &in) {
    std::vector<uint8_t> code;
    std::string leftVar, comp, rightVal;
    iss >> leftVar >> comp >> rightVal;
    int constant = std::stoi(rightVal);
    code.push_back(OP_LOAD_VAR);
    code.push_back(static_cast<uint8_t>(leftVar[0]));
    code.push_back(OP_PUSH);
    code.push_back(static_cast<uint8_t>(constant));
    code.push_back(OP_JUMP_IF_ZERO);
    code.push_back(0x00);
    code.push_back(0x00);
    std::vector<uint8_t> loopBlock = compileBlock(in);
    code.insert(code.end(), loopBlock.begin(), loopBlock.end());
    code.push_back(OP_JUMP);
    code.push_back(0x00);
    code.push_back(0x00);
    return code;
}

// Modified compileBlock to dispatch statements by first keyword.
std::vector<uint8_t> compileBlock(std::istream &in) {
    std::vector<uint8_t> blockCode;
    std::string line;
    while(getline(in, line)) {
        line = trim(line);
        if(!line.empty() && line[0]=='}') break;
        if(line.empty() || line.substr(0,2)=="//") continue;
        std::istringstream iss(line);
        std::string firstToken;
        iss >> firstToken;
        std::string tokenLower = firstToken;
        std::transform(tokenLower.begin(), tokenLower.end(), tokenLower.begin(), ::tolower);
        if(tokenLower=="if") {
            std::vector<uint8_t> part = compileIfStatement(iss, in);
            blockCode.insert(blockCode.end(), part.begin(), part.end());
        } else if(tokenLower=="while") {
            std::vector<uint8_t> part = compileWhileStatement(iss, in);
            blockCode.insert(blockCode.end(), part.begin(), part.end());
        } else if(tokenLower=="let") {
            std::vector<uint8_t> part = compileAssignment(iss);
            blockCode.insert(blockCode.end(), part.begin(), part.end());
        } else if(tokenLower=="print" || tokenLower=="printnl") {
            // ...existing print handling...
            std::string operand;
            getline(iss, operand);
            operand = trim(operand);
            if(!operand.empty() && operand.back()==';')
                operand.pop_back();
            operand = trim(operand);
            if(operand.find('+') != std::string::npos) {
                size_t start = 0;
                while(true) {
                    size_t pos = operand.find('+', start);
                    std::string token = (pos==std::string::npos) ? operand.substr(start) : operand.substr(start, pos-start);
                    token = trim(token);
                    if(token.size()>=2 && token.front()=='\"' && token.back()=='\"') {
                        std::string text = token.substr(1, token.size()-2);
                        for(char c : text) {
                            blockCode.push_back(OP_PUSH);
                            blockCode.push_back(static_cast<uint8_t>(c));
                            blockCode.push_back(OP_PRINT);
                        }
                    } else if(!token.empty()){
                        blockCode.push_back(PRINT_VAR);
                        blockCode.push_back(static_cast<uint8_t>(token[0]));
                    }
                    if(pos==std::string::npos)
                        break;
                    start = pos+1;
                }
            } else {
                if(operand.size()<2 || operand.front()!='\"' || operand.back()!='\"')
                    throw std::runtime_error("Expected quoted string in block: " + line);
                std::string text = operand.substr(1, operand.size()-2);
                for(char c : text) {
                    blockCode.push_back(OP_PUSH);
                    blockCode.push_back(static_cast<uint8_t>(c));
                    blockCode.push_back(OP_PRINT);
                }
            }
            if(tokenLower=="printnl")
                blockCode.push_back(OP_PRINTLN);
        }
        // ...existing code for other statements...
    }
    return blockCode;
}

// NEW: Revised main() to support the new syntax for test.covi.
int main(int argc, char* argv[]) {
    if(argc != 4) {
        cerr << "Usage: covicc <input.covi> -o <output.cb>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    if(string(argv[2]) != "-o") {
        cerr << "Usage: covicc <input.covi> -o <output.cb>" << endl;
        return 1;
    }
    string outputFile = argv[3];

    ifstream fin(inputFile);
    if(!fin) {
        cerr << "Error: Cannot open input file " << inputFile << endl;
        return 1;
    }
    // Read entire file into vector of strings.
    vector<string> lines;
    string line;
    while(getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();
    
    // Look for the main block in the new syntax.
    size_t mainIndex = 0;
    bool foundMain = false;
    for (size_t i = 0; i < lines.size(); i++){
        if(lines[i].find("main") != string::npos && lines[i].find("{") != string::npos){
            mainIndex = i;
            foundMain = true;
            break;
        }
    }
    if(!foundMain) {
        cerr << "Error: main block not found in new syntax" << endl;
        return 1;
    }
    // Find the opening brace in the main block line.
    size_t bracePos = lines[mainIndex].find('{');
    if(bracePos == string::npos) {
        cerr << "Error: Opening brace not found in main block" << endl;
        return 1;
    }
    // Build a string stream containing the block, starting with the remainder of the main line.
    ostringstream oss;
    oss << lines[mainIndex].substr(bracePos + 1) << "\n";
    int openBraces = 1;
    for(size_t i = mainIndex + 1; i < lines.size(); i++){
        for(char c : lines[i]) {
            if(c == '{')
                openBraces++;
            else if(c == '}')
                openBraces--;
        }
        if(openBraces <= 0)
            break;
        oss << lines[i] << "\n";
    }
    istringstream blockStream(oss.str());
    
    // Begin bytecode generation.
    vector<uint8_t> bytecode;
    // Write custom magic number header (big-endian).
    bytecode.push_back((CUSTOM_MAGIC >> 24) & 0xFF);
    bytecode.push_back((CUSTOM_MAGIC >> 16) & 0xFF);
    bytecode.push_back((CUSTOM_MAGIC >> 8) & 0xFF);
    bytecode.push_back(CUSTOM_MAGIC & 0xFF);
    
    vector<uint8_t> blockCode = compileBlock(blockStream);
    bytecode.insert(bytecode.end(), blockCode.begin(), blockCode.end());
    
    ofstream fout(outputFile, ios::binary);
    if(!fout) {
        cerr << "Error: Cannot open output file " << outputFile << endl;
        return 1;
    }
    fout.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    fout.close();
    
    cout << "Compilation completed: " << outputFile << endl;
    return 0;
}
