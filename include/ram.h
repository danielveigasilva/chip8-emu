#include <chip8.h>

#define MEM_SIZE 4096
#define MEM_ADDR_START_PROG 0x200
#define MEM_ADDR_START_FONT 0x000
#define MEM_FONT_SIZE_BYTES 5
#define MEM_NUM_CHARS_FONT 16

void ram_init_ctx(Chip8_t* ctx);

int ram_load_rom_to_mem(Chip8_t* ctx, char* filename);

void ram_write_mem(Chip8_t* ctx, u_int16_t addr, u_int8_t data);

u_int8_t ram_read_mem(Chip8_t* ctx, u_int16_t addr);

u_int16_t ram_addr_font_sprit_in_mem(u_int16_t character);