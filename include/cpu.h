#include <chip8.h>

void init_cpu_ctx(Chip8_t* ctx);
void fetch_decode_execute_instruction(Chip8_t* ctx);
void update_timers(Chip8_t* ctx);
