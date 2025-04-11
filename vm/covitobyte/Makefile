CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SRC = covicc.cpp
TARGET = covicc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
