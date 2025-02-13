#include <stdio.h>
#include <chip8.h>

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define DISPLAY_SCALE 20

void init_gpu_ctx(Chip8_t* ctx);

u_int8_t draw_sprit(Chip8_t* ctx, u_int8_t col, u_int8_t row, u_int16_t addr_init_sprit, u_int16_t n);

void clear_display(Chip8_t* ctx);