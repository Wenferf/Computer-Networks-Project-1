# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Target executables
TARGETS = echoserver echoclient

# Default rule to build everything
all: $(TARGETS)

# Rule to build the server executable
echoserver: $(OBJ_DIR)/echoserver.o
	$(CC) $(CFLAGS) -o $@ $<

# Rule to build the client executable
echoclient: $(OBJ_DIR)/echoclient.o
	$(CC) $(CFLAGS) -o $@ $<


# Generic rule to compile .c files from src/ into .o files in obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Order-only prerequisite to ensure obj/ directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGETS)

#	.PHONY: all clean
