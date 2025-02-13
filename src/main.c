#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>
#include <cpu.h>
#include <ram.h>
#include <gpu.h>
#include <helper.h>
#include <input.h>
#include <SDL2/SDL.h>


void chip8_init_ctx(Chip8_t* ctx){
    ram_init_ctx(ctx);
    cpu_init_ctx(ctx);
    gpu_init_ctx(ctx);
    input_init_ctx(ctx);
}

int main(int argc, char **argv) {

    Chip8_t ctx;
    chip8_init_ctx(&ctx);
    ctx.input.keys = (u_int8_t*) calloc(16, sizeof(u_int8_t));

    if (ram_load_rom_to_mem(&ctx, argv[1]))
        return 1;

    ctx.state = CHIP8_RUNNING;

    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(DISPLAY_WIDTH*DISPLAY_SCALE, DISPLAY_HEIGHT*DISPLAY_SCALE, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "CHIP-8");
    SDL_RenderSetScale(renderer, DISPLAY_SCALE, DISPLAY_SCALE);

    while(ctx.state != CHIP8_STOPPED){
        
        while (SDL_PollEvent(&event) != 0)
            input_handle_key_event(&ctx, event);

        if (ctx.state == CHIP8_PAUSED || ctx.state == CHIP8_STOPPED)
            continue;

        for (int i = 0; i < 700/60; i++)
            cpu_fetch_decode_execute_instruction(&ctx);
        
        if (ctx.gpu.update_display){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int i = 0; i < DISPLAY_HEIGHT; ++i)
                for (int j = 0; j < DISPLAY_WIDTH; ++j)
                    if (ctx.gpu.display[i][j])
                        SDL_RenderDrawPoint(renderer, j, i);
            
            SDL_RenderPresent(renderer);
            ctx.gpu.update_display = 0;
        }

        SDL_Delay(16);
        cpu_update_timers(&ctx);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}