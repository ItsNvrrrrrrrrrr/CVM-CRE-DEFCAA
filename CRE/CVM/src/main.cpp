#include <iostream>
#include <fstream>
#include <vector>
#include "vm.h"

int main(int argc, char* argv[]) {
    // Kiểm tra số lượng đối số
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <bytecode_file>" << std::endl;
        return 1;
    }
    
    try {
        runVM(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}