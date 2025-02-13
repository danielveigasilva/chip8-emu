#include <chip8.h>

void cpu_init_ctx(Chip8_t* ctx);
void cpu_fetch_decode_execute_instruction(Chip8_t* ctx);
void cpu_update_timers(Chip8_t* ctx);
