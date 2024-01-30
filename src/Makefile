# Name of the executable
TARGET = main

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Linker flags
LDFLAGS = -lncurses

# Source files
SRC = main.c

# Build rule for the main target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean rule to remove generated files
clean:
	rm -f $(TARGET)

