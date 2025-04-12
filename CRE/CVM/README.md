# DEFCAA bytecode format project

This project implements a simple virtual machine (VM) capable of executing bytecode files. The VM can handle two types of bytecode: custom bytecode and Java bytecode. 

## Project Structure

```
my-vm-app
├── src
│   ├── main.cpp        # Entry point of the VM
│   ├── vm.cpp          # Implementation of the VM
│   ├── vm.h            # Header file for VM functions and classes
│   └── utils.cpp       # Utility functions for file handling and error checking
├── bytecode
│   ├── custom_sample.bc # Sample custom bytecode file
│   └── java_sample.class # Sample Java bytecode file
├── Makefile             # Build instructions for the project
└── README.md            # Documentation for the project
```

## Build Instructions

To build the project, navigate to the project directory and run the following command:

```bash
make
```

This will compile the source files and create the executable for the virtual machine.

## How to Test the VM

1. **Custom Bytecode**: 
   - Use the provided `bytecode/custom_sample.bc` file to test the custom bytecode execution. 
   - Run the VM with the command:
     ```bash
     ./my-vm-app <path_to_custom_bytecode>
     ```

2. **Java Bytecode**: 
   - Use the provided `bytecode/java_sample.class` file to test the Java bytecode detection.
   - Run the VM with the command:
     ```bash
     ./my-vm-app <path_to_java_bytecode>
     ```

## Sample Bytecode Files

- **custom_sample.bc**: This file contains a sequence of custom opcodes that the VM can execute. It demonstrates the basic operations supported by the custom bytecode.

- **java_sample.class**: This file is a sample Java bytecode file. When detected, the VM will print a message indicating that Java bytecode is detected.

## Error Handling

The VM includes error handling for:
- Invalid magic numbers
- Stack overflow and underflow
- Unsupported opcodes

## Notes

This project is a minimal implementation of a virtual machine and does not cover all aspects of a full JVM. The Java bytecode handling is limited to detecting the bytecode and printing a message. Further enhancements can be made to support more Java bytecode features.
