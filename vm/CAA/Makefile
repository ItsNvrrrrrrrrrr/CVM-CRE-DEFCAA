CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SRC = src/main.cpp src/vm.cpp src/utils.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = cvm
DEFASM = defasm

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

defasm: src/defasm.cpp
	$(CC) $(CFLAGS) -o $(DEFASM) src/defasm.cpp

clean:
	rm -f $(OBJ) $(TARGET) $(DEFASM)

run: $(TARGET)
	./$(TARGET) bytecode/java_sample.class

test: $(TARGET)
	./$(TARGET) bytecode/custom_sample.bc

.PHONY: all clean run test defasm