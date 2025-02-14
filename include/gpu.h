#include <chip8.h>

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define DISPLAY_SCALE 10

void gpu_init_ctx(Chip8_t* ctx);

u_int8_t gpu_draw_sprit(Chip8_t* ctx, u_int8_t col, u_int8_t row, u_int16_t addr_init_sprit, u_int16_t n);

void gpu_clear_display(Chip8_t* ctx);