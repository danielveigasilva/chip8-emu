#include <graphic.h>
#include <stdio.h>
#include <memory.h>
#include <helper.h>

u_int8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT];

void clear_display(){
    for (int i=0; i < DISPLAY_WIDTH; i++)
        for (int j=0; j < DISPLAY_HEIGHT; j++)
            display[i][j] = 0;
}

void print_display(){
    for (int i=0; i < DISPLAY_WIDTH; i++){
        for (int j=0; j < DISPLAY_HEIGHT; j++)
            printf("%s", display[i][j] == 1 ? DISPLAY_PIXEL_SET : DISPLAY_PIXEL_UNSET);
        printf("\n");
    }
}

u_int8_t draw_sprit(u_int8_t col, u_int8_t row, u_int16_t addr_init_sprit, u_int16_t n){
    
    col = col % DISPLAY_HEIGHT;
    row = row % DISPLAY_WIDTH;
    
    u_int8_t flag = 0;
    for (int i=0; i < n && row+i < DISPLAY_WIDTH; i++){
        u_int8_t new_sprit_byte = read_mem(addr_init_sprit + i);
        
        u_int8_t old_sprit_byte = 0;
        for (int j=0; j < 8 && col+j < DISPLAY_HEIGHT; j++)
            old_sprit_byte |= display[row+i][col+j] << (7-j);
        
        u_int8_t result_sprit_byte = new_sprit_byte ^ old_sprit_byte;
        for (int j=0; j < 8 & col+j < DISPLAY_HEIGHT; j++)
            display[row+i][col+j] = 0b00000001 & (result_sprit_byte >> (7-j));

        flag |= old_sprit_byte & ~result_sprit_byte;
    }
    return flag > 0 ? 1 : 0;
}