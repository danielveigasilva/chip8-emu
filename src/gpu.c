#include <gpu.h>
#include <ram.h>

void gpu_init_ctx(Chip8_t* ctx){
    ctx->gpu.display = (u_int8_t**) calloc(DISPLAY_HEIGHT, sizeof(u_int8_t*));
    for (int i=0; i < DISPLAY_HEIGHT; i++)
        ctx->gpu.display[i] = (u_int8_t*) calloc(DISPLAY_WIDTH, sizeof(u_int8_t));
}

u_int8_t gpu_draw_sprit(Chip8_t* ctx, u_int8_t col, u_int8_t row, u_int16_t addr_init_sprit, u_int16_t n){
     
    row = row % DISPLAY_HEIGHT;
    col = col % DISPLAY_WIDTH;
    
    u_int8_t flag = 0;
    for (int i=0; i < n && row+i < DISPLAY_HEIGHT; i++){
        u_int8_t new_sprit_byte = ram_read_mem(ctx, addr_init_sprit + i);
        
        u_int8_t old_sprit_byte = 0;
        for (int j=0; j < 8 && col+j < DISPLAY_WIDTH; j++)
            old_sprit_byte |= ctx->gpu.display[row+i][col+j] << (7-j);
        
        u_int8_t result_sprit_byte = new_sprit_byte ^ old_sprit_byte;
        for (int j=0; j < 8 & col+j < DISPLAY_WIDTH; j++)
            ctx->gpu.display[row+i][col+j] = 0b00000001 & (result_sprit_byte >> (7-j));

        flag |= old_sprit_byte & ~result_sprit_byte;
    }
    ctx->gpu.update_display = 1;
    return flag > 0 ? 1 : 0;
}

void gpu_clear_display(Chip8_t* ctx){
    for (int i = 0; i < DISPLAY_HEIGHT; i++)
        for (int j = 0; j < DISPLAY_WIDTH; j++)
            ctx->gpu.display[i][j] = 0;
    ctx->gpu.update_display = 1;
}