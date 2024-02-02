# Name of the executable
TARGET = pnck 

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Linker flags
LDFLAGS = -lncurses

# Source files directory
SRC_DIR = src

# Source files
SRC = $(SRC_DIR)/main.c

# Object files directory
OBJ_DIR = $(SRC_DIR)/obj

# Build rule for the main target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean rule to remove generated files
clean:
	rm -f $(TARGET)

