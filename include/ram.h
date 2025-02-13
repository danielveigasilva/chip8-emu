#include <chip8.h>

#define MEM_SIZE 4096
#define MEM_ADDR_START_PROG 0x200
#define MEM_ADDR_START_FONT 0x000
#define MEM_FONT_SIZE_BYTES 5
#define MEM_NUM_CHARS_FONT 16

void init_ram_ctx(Chip8_t* ctx);

int load_rom_to_mem(Chip8_t* ctx, char* filename);

void write_mem(Chip8_t* ctx, u_int16_t addr, u_int8_t data);

u_int8_t read_mem(Chip8_t* ctx, u_int16_t addr);

u_int16_t addr_font_sprit_in_mem(u_int16_t character);