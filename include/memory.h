#include <stdio.h>

#define MEM_SIZE 4096
#define MEM_ADDR_START_PROG 0x200
#define MEM_ADDR_START_FONT 0x000
#define MEM_FONT_SIZE_BYTES 5
#define MEM_NUM_CHARS_FONT 16

extern u_int8_t memory[];

void load_font_sprits_to_mem();

u_int16_t addr_font_sprit_in_mem(u_int16_t character);

int load_rom_to_mem(char* filename);

void write_mem(u_int16_t addr, u_int8_t data);

u_int8_t read_mem(u_int16_t addr);