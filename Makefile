# Biến cấu hình
CC = gcc
CFLAGS = -Wall -Wextra -I./CRE/compiler -I./CRE/vm -I./CRE/lib
LDFLAGS =

# Danh sách source cho compiler và VM
COMPILER_SRCS = CRE/compiler/covicc.c CRE/compiler/lexer.c CRE/compiler/parser.c CRE/compiler/codegen.c CRE/compiler/utils.c
VM_SRCS = CRE/vm/covim.c CRE/vm/runtime.c CRE/vm/bytecode.c

# Tạo file object tương ứng
COMPILER_OBJS = $(COMPILER_SRCS:.c=.o)
VM_OBJS = $(VM_SRCS:.c=.o)

# Biến cho cblio object
COVILIB = /workspaces/CVM-CRE-DEFCAA/Covi1/colib/cblio.o

# Target mặc định: compile cả compiler và VM
all: covicc covim

# Build compiler - use g++ for covicc.c to enable C++ headers
covicc: $(COMPILER_OBJS)
	g++ $(CFLAGS) -o covicc $(COMPILER_OBJS) $(LDFLAGS)

# Build virtual machine
covim: CRE/vm/covim.o CRE/vm/runtime.o CRE/vm/bytecode.o $(COVILIB)
	$(CC) $(CFLAGS) -o covim CRE/vm/covim.o CRE/vm/runtime.o CRE/vm/bytecode.o $(COVILIB)

# Build cblio.o from cblio.c
$(COVILIB): /workspaces/CVM-CRE-DEFCAA/Covi1/colib/cblio.c
	$(CC) $(CFLAGS) -c /workspaces/CVM-CRE-DEFCAA/Covi1/colib/cblio.c -o $(COVILIB)

# For C++ source, force the file to be treated as C++ even if extension is .c.
CRE/compiler/covicc.o: CRE/compiler/covicc.c
	g++ $(CFLAGS) -x c++ -c $< -o $@

# Quy tắc build .o từ .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f covicc covim $(COMPILER_OBJS) $(VM_OBJS)
