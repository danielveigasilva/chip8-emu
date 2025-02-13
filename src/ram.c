#include <stdlib.h>
#include <ram.h>

u_int8_t font_sprits[MEM_FONT_SIZE_BYTES*MEM_NUM_CHARS_FONT] = {
    0xF0,0x90,0x90,0x90,0xF0, //0
    0x20,0x60,0x20,0x20,0x70, //1
    0xF0,0x10,0xF0,0x80,0xF0, //2
    0xF0,0x10,0xF0,0x10,0xF0, //3
    0x90,0x90,0xF0,0x10,0x10, //4
    0xF0,0x80,0xF0,0x10,0xF0, //5
    0xF0,0x80,0xF0,0x90,0xF0, //6
    0xF0,0x10,0x20,0x40,0x40, //7
    0xF0,0x90,0xF0,0x90,0xF0, //8
    0xF0,0x90,0xF0,0x10,0xF0, //9
    0xF0,0x90,0xF0,0x90,0x90, //A
    0xE0,0x90,0xE0,0x90,0xE0, //B
    0xF0,0x80,0x80,0x80,0xF0, //C
    0xE0,0x90,0x90,0x90,0xE0, //D
    0xF0,0x80,0xF0,0x80,0xF0, //E
    0xF0,0x80,0xF0,0x80,0x80  //F
};

void ram_init_ctx(Chip8_t* ctx){
    ctx->ram.memory = (u_int8_t*) calloc(MEM_SIZE, sizeof(u_int8_t));
    //Load font sprits to ram
    for (int i=MEM_ADDR_START_FONT; i<(MEM_FONT_SIZE_BYTES*MEM_NUM_CHARS_FONT)+MEM_ADDR_START_FONT; i++)
        ctx->ram.memory[i] = font_sprits[i];
}

int ram_load_rom_to_mem(Chip8_t* ctx, char* filename){
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return 1;

    u_int8_t * ram_slot_prog = ctx->ram.memory + MEM_ADDR_START_PROG;
    int ram_space_prog_left = MEM_SIZE - MEM_ADDR_START_PROG + 1;
    fread(ram_slot_prog, sizeof(u_int8_t), ram_space_prog_left, file);
    fclose(file);

    return 0;
}

void ram_write_mem(Chip8_t* ctx, u_int16_t addr, u_int8_t data){
    ctx->ram.memory[addr] = data;
}

u_int8_t ram_read_mem(Chip8_t* ctx, u_int16_t addr){
    return ctx->ram.memory[addr];
}

u_int16_t ram_addr_font_sprit_in_mem(u_int16_t character){
    return character * MEM_FONT_SIZE_BYTES + MEM_ADDR_START_FONT;
}