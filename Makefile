CC = gcc
CFLAGS = -Iinclude -Wall -g
SRC = src/main.c src/cpu.c src/memory.c src/helper.c src/graphic.c
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = bin/chip8-emu
BUILD_DIR = build
BIN_DIR = bin

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(TARGET) $(OBJ)

build/%.o: src/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

