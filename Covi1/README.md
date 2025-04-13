# Covi Project

## Overview
Covi is a programming language designed for simplicity and ease of use. This project includes a compiler (`Covicc`) and a virtual machine (`Covim`) for executing programs written in Covi. The language supports basic constructs such as functions, return types, and standard input/output operations.

## Project Structure
The project is organized into the following directories:

- **compiler/**: Contains the source code for the Covicc compiler.
  - `covicc.c`: Main entry point for the compiler.
  - `lexer.c` / `lexer.h`: Implements the lexer for tokenizing source code.
  - `parser.c` / `parser.h`: Implements the parser for constructing the abstract syntax tree (AST).
  - `codegen.c` / `codegen.h`: Translates the AST into bytecode.
  - `utils.c` / `utils.h`: Contains utility functions for error handling and memory management.

- **vm/**: Contains the source code for the Covim virtual machine.
  - `covim.c`: Main entry point for the virtual machine.
  - `bytecode.c` / `bytecode.h`: Handles reading and interpreting the bytecode format.
  - `runtime.c` / `runtime.h`: Manages the runtime environment, including stack management and execution of bytecode.

- **lib/**: Contains the standard library for the Covi language.
  - `cblio.covil`: Provides built-in functions and utilities for Covi programs.

- **test/**: Contains test cases and scripts for testing the compiler and VM.
  - `hello.covi`: A sample Covi program demonstrating the language syntax.
  - `test_fail.covi`: A sample program designed to trigger syntax errors.
  - `compile_and_run.sh`: A script to automate the compilation and execution of Covi programs.

## Usage

### Compile Source Code
To compile a Covi source file into bytecode, use the following command:
```
./covicc path/to/source.covi -o path/to/output.fac
```
Example:
```
./covicc test/hello.covi -o out/hello.fac
```

### Run Bytecode
To execute a compiled bytecode file, use the following command:
```
./covim path/to/file.fac
```
Example:
```
./covim out/hello.fac
```

### Automated Testing
You can use the provided script to compile and run a Covi program automatically:
```
./test/compile_and_run.sh test/hello.covi
```

## Features
- Support for public and private functions.
- Functions can return `void` or `int`.
- Standard library functions available in `lib/`.
- Stack simulation in the VM.
- Debug logging in the VM when the `--debug` flag is enabled.

## Contributing
Contributions to the Covi project are welcome! Please feel free to submit issues or pull requests.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.