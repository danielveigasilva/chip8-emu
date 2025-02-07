#include <stdio.h>

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 32
#define DISPLAY_PIXEL_SET " ■ "
#define DISPLAY_PIXEL_UNSET "   "

extern u_int8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT];

void clear_display();

void print_display();

u_int8_t draw_sprit(u_int8_t col, u_int8_t row, u_int16_t addr_init_sprit, u_int16_t n);