# Makefile to compile all .s files in the current directory to .bin files

# Set the RISC-V cross-compiler path
RISCV_PATH = /Users/xim/Documents/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-apple-darwin/

# Set the compiler and linker flags
CFLAGS = -march=rv64i -mabi=lp64 -O0 -g
LDFLAGS = -nostdlib -nostartfiles -Ttext 0x80000000

# Define the source and object file extensions
SRC_EXT = s
OBJ_EXT = elf
BIN_EXT = bin

# Get a list of all source files in the current directory
SRCS = $(wildcard *.$(SRC_EXT))

# Generate a list of all object files to be created
OBJS = $(SRCS:.$(SRC_EXT)=.$(OBJ_EXT))

# Generate a list of all binary files to be created
BINS = $(SRCS:.$(SRC_EXT)=.$(BIN_EXT))

# Define the default target
all: $(BINS)

# Define the rule to build an object file from a source file
%.$(OBJ_EXT): %.$(SRC_EXT)
	$(RISCV_PATH)/bin/riscv64-unknown-elf-gcc $(CFLAGS) $(LDFLAGS) $< -o $@

# Define the rule to build a binary file from an ELF file
%.$(BIN_EXT): %.$(OBJ_EXT)
	$(RISCV_PATH)/bin/riscv64-unknown-elf-objcopy -O binary $< $@

# Define the clean target
clean:
	rm -f *.$(OBJ_EXT) *.$(BIN_EXT)

# Commands used:
# None