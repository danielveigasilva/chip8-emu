#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>
#include <cpu.h>
#include <ram.h>
#include <gpu.h>
#include <helper.h>
#include <SDL2/SDL.h>


void init_chip8_ctx(Chip8_t* ctx){
    init_ram_ctx(ctx);
    init_cpu_ctx(ctx);
    init_gpu_ctx(ctx);
}

int main(int argc, char **argv) {

    Chip8_t ctx;
    init_chip8_ctx(&ctx);
    ctx.input.keys = (u_int8_t*) calloc(16, sizeof(u_int8_t));

    if (load_rom_to_mem(&ctx, argv[1]))
        return 1;

    ctx.state = CHIP8_RUNNING;

    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(DISPLAY_WIDTH*DISPLAY_SCALE, DISPLAY_HEIGHT*DISPLAY_SCALE, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, DISPLAY_SCALE, DISPLAY_SCALE);

    while(ctx.state != CHIP8_STOPPED){
        
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type)
            {
                case SDL_QUIT:
                    ctx.state = CHIP8_STOPPED;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_SPACE:
                            ctx.state = ctx.state == CHIP8_PAUSED ? CHIP8_RUNNING : CHIP8_PAUSED;
                            break;
                        case SDLK_0:
                            ctx.input.keys[0x0] = 1;
                            break;
                        case SDLK_1:
                            ctx.input.keys[0x1] = 1;
                            break;
                        case SDLK_2:
                            ctx.input.keys[0x2] = 1;
                            break;
                        case SDLK_3:
                            ctx.input.keys[0x3] = 1;
                            break;
                        case SDLK_4:
                            ctx.input.keys[0x4] = 1;
                            break;
                        case SDLK_5:
                            ctx.input.keys[0x5] = 1;
                            break;
                        case SDLK_6:
                            ctx.input.keys[0x6] = 1;
                            break;
                        case SDLK_7:
                            ctx.input.keys[0x7] = 1;
                            break;
                        case SDLK_8:
                            ctx.input.keys[0x8] = 1;
                            break;
                        case SDLK_9:
                            ctx.input.keys[0x9] = 1;
                            break;
                        case SDLK_a:
                            ctx.input.keys[0xa] = 1;
                            break;
                        case SDLK_b:
                            ctx.input.keys[0xb] = 1;
                            break;
                        case SDLK_c:
                            ctx.input.keys[0xc] = 1;
                            break;
                        case SDLK_d:
                            ctx.input.keys[0xd] = 1;
                            break;
                        case SDLK_e:
                            ctx.input.keys[0xe] = 1;
                            break;
                        case SDLK_f:
                            ctx.input.keys[0xf] = 1;
                            break;
                        default:
                            break;                    
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_0:
                            ctx.input.keys[0x0] = 0;
                            break;
                        case SDLK_1:
                            ctx.input.keys[0x1] = 0;
                            break;
                        case SDLK_2:
                            ctx.input.keys[0x2] = 0;
                            break;
                        case SDLK_3:
                            ctx.input.keys[0x3] = 0;
                            break;
                        case SDLK_4:
                            ctx.input.keys[0x4] = 0;
                            break;
                        case SDLK_5:
                            ctx.input.keys[0x5] = 0;
                            break;
                        case SDLK_6:
                            ctx.input.keys[0x6] = 0;
                            break;
                        case SDLK_7:
                            ctx.input.keys[0x7] = 0;
                            break;
                        case SDLK_8:
                            ctx.input.keys[0x8] = 0;
                            break;
                        case SDLK_9:
                            ctx.input.keys[0x9] = 0;
                            break;
                        case SDLK_a:
                            ctx.input.keys[0xa] = 0;
                            break;
                        case SDLK_b:
                            ctx.input.keys[0xb] = 0;
                            break;
                        case SDLK_c:
                            ctx.input.keys[0xc] = 0;
                            break;
                        case SDLK_d:
                            ctx.input.keys[0xd] = 0;
                            break;
                        case SDLK_e:
                            ctx.input.keys[0xe] = 0;
                            break;
                        case SDLK_f:
                            ctx.input.keys[0xf] = 0;
                            break;
                        default:
                            break;                    
                    }
                break;
                default:
                    break;
            }
        }

        if (ctx.state == CHIP8_PAUSED || ctx.state == CHIP8_STOPPED)
            continue;

        for (int i = 0; i < 700/60; i++)
            fetch_decode_execute_instruction(&ctx);
        
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
        update_timers(&ctx);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}