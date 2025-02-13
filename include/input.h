#include <chip8.h>
#include <SDL2/SDL.h>

void input_init_ctx(Chip8_t* ctx);

void input_handle_key_event(Chip8_t* ctx, SDL_Event event);