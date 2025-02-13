#include <input.h>
#include <stdlib.h>

SDL_Keycode input_remap_keys(u_int8_t key){
    switch (key)
    {
        case 0x1:
            return SDLK_1;
        case 0x2:
            return SDLK_2;
        case 0x3:
            return SDLK_3;
        case 0xC:
            return SDLK_4;

        case 0x4:
            return SDLK_q;
        case 0x5:
            return SDLK_w;
        case 0x6:
            return SDLK_e;
        case 0xD:
            return SDLK_r;

        case 0x7:
            return SDLK_a;
        case 0x8:
            return SDLK_s;
        case 0x9:
            return SDLK_d;
        case 0xE:
            return SDLK_f;

        case 0xA:
            return SDLK_z;
        case 0x0:
            return SDLK_x;
        case 0xB:
            return SDLK_c;
        case 0xF:
            return SDLK_v;
        
        default:
            return SDLK_UNKNOWN;
    }
}

void input_init_ctx(Chip8_t* ctx){
    ctx->input.keys = (u_int8_t *) calloc(16, sizeof(u_int8_t));
}

void input_handle_key_event(Chip8_t* ctx, SDL_Event event){
    
    switch (event.type)
    {
        case SDL_QUIT:
            ctx->state = CHIP8_STOPPED;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_SPACE:
                    ctx->state = ctx->state == CHIP8_PAUSED ? CHIP8_RUNNING : CHIP8_PAUSED;
                    break;
                default:
                    for (u_int8_t i = 0; i < 16; i++)
                        if (event.key.keysym.sym == input_remap_keys(i))
                            ctx->input.keys[i] = 1;
                    break;                    
            }
            break;
        case SDL_KEYUP:
            for (u_int8_t i = 0; i < 16; i++)
                if (event.key.keysym.sym == input_remap_keys(i))
                    ctx->input.keys[i] = 0;
        break;
        default:
            break;
    }
}