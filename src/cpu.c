#include <cpu.h>
#include <ram.h>
#include <gpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GET_X(opcode) (((opcode) & 0x0F00) >> 8)
#define GET_Y(opcode) (((opcode) & 0x00F0) >> 4)
#define GET_N(opcode) ((opcode) & 0x000F)
#define GET_NN(opcode) ((u_int8_t)((opcode) & 0x00FF))
#define GET_NNN(opcode) ((opcode) & 0x0FFF)

void cpu_init_ctx(Chip8_t* ctx){
    ctx->cpu.I = 0;
    ctx->cpu.sp = 0;
    ctx->cpu.dt = 0;
    ctx->cpu.st = 0;
    ctx->cpu.pc = MEM_ADDR_START_PROG;
    ctx->cpu.V = (u_int8_t*) calloc(16, sizeof(u_int8_t));
    ctx->cpu.callstack = (u_int16_t*) calloc(16, sizeof(u_int16_t));
}

void cpu_fetch_decode_execute_instruction(Chip8_t* ctx){
    ctx->cpu.opcode = ram_read_mem(ctx, ctx->cpu.pc) << 8 | ram_read_mem(ctx, ctx->cpu.pc + 1);
    switch (ctx->cpu.opcode & 0xF000){
        case 0x0000:
            switch (ctx->cpu.opcode & 0x0FFF){
                case 0x00E0:
                    //(00E0) Clears the screen
                    gpu_clear_display(ctx);
                    ctx->cpu.pc += 2;
                    break;
                case 0x00EE:
                    //(00EE) Returns from a subroutine
                    ctx->cpu.pc = ctx->cpu.callstack[--ctx->cpu.sp];
                    ctx->cpu.pc += 2;
                    break;
                default: 
                    //(0NNN) Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN
                    ctx->cpu.pc += 2;
                    break;
            }
            break;
        case 0x1000:
            //(1NNN) Jumps to address NNN
            ctx->cpu.pc = GET_NNN(ctx->cpu.opcode);
            break;
        case 0x2000:
            //(2NNN) Calls subroutine at NNN
            ctx->cpu.callstack[ctx->cpu.sp++] = ctx->cpu.pc;
            ctx->cpu.pc = GET_NNN(ctx->cpu.opcode);
            break;
        case 0x3000:
            //(3XNN) Skips the next instruction if VX equals NN
            ctx->cpu.pc += 2;
            if (ctx->cpu.V[GET_X(ctx->cpu.opcode)] == GET_NN(ctx->cpu.opcode))
                ctx->cpu.pc += 2;
            break;
        case 0x4000:
            //(4XNN) Skips the next instruction if VX does not equal NN
            ctx->cpu.pc += 2;
            if (ctx->cpu.V[GET_X(ctx->cpu.opcode)] != GET_NN(ctx->cpu.opcode))
                ctx->cpu.pc += 2;
            break;
        case 0x5000:
            //(5XY0) Skips the next instruction if VX equals VY
            ctx->cpu.pc += 2;
            if (ctx->cpu.V[GET_X(ctx->cpu.opcode)] == ctx->cpu.V[GET_Y(ctx->cpu.opcode)])
                ctx->cpu.pc += 2;
            break;
        case 0x6000:
            //(6XNN) Sets VX to NN
            ctx->cpu.V[GET_X(ctx->cpu.opcode)] = GET_NN(ctx->cpu.opcode);
            ctx->cpu.pc += 2;
            break;
        case 0x7000:
            //(7XNN) Adds NN to VX (carry flag is not changed)
            ctx->cpu.V[GET_X(ctx->cpu.opcode)] += GET_NN(ctx->cpu.opcode);
            ctx->cpu.pc += 2;
            break;
        case 0x8000:
            switch (ctx->cpu.opcode & 0x000F){
                case 0x0000:
                    //(8XY0) Sets VX to the value of VY
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] = ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0001:
                    //(8XY1) Sets VX to VX or VY
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] |= ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0002:
                    //(8XY2) Sets VX to VX and VY
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] &= ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0003:
                    //(8XY3) Sets VX to VX xor VY
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] ^= ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0004:
                    //(8XY4) Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
                    ctx->cpu.V[0xF] = (u_int8_t) ((((u_int16_t) ctx->cpu.V[GET_X(ctx->cpu.opcode)] + (u_int16_t) ctx->cpu.V[GET_Y(ctx->cpu.opcode)]) & 0xF00) >> 8); //TODO: Review overflow bit
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] += ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0005:
                    //(8XY5) VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] -= ctx->cpu.V[GET_Y(ctx->cpu.opcode)];
                    ctx->cpu.V[0xF] = ctx->cpu.V[GET_X(ctx->cpu.opcode)] >= ctx->cpu.V[GET_Y(ctx->cpu.opcode)] ? 0 : 1;
                    ctx->cpu.pc += 2;
                    break;
                case 0x0006:
                    //(8XY6) Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
                    ctx->cpu.V[0xF] = ctx->cpu.V[GET_X(ctx->cpu.opcode)] & 0b00000001;
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] >>= 1;
                    ctx->cpu.pc += 2;
                    break;
                case 0x0007:
                    //(8XY7) Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] = ctx->cpu.V[GET_Y(ctx->cpu.opcode)] - ctx->cpu.V[GET_X(ctx->cpu.opcode)];
                    ctx->cpu.V[0xF] = ctx->cpu.V[GET_Y(ctx->cpu.opcode)] < ctx->cpu.V[GET_X(ctx->cpu.opcode)] ? 0 : 1;
                    ctx->cpu.pc += 2;
                    break;
                case 0x000E:
                    //(8XYE) Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
                    ctx->cpu.V[0xF] = (ctx->cpu.V[GET_X(ctx->cpu.opcode)] & 0b10000000) >> 7;
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] <<= 1;
                    ctx->cpu.pc += 2;
                    break;
                default: 
                    break;
            }
            break;
        case 0x9000:
            //(9XY0) Skips the next instruction if VX does not equal VY
            if (ctx->cpu.V[GET_X(ctx->cpu.opcode)] != ctx->cpu.V[GET_Y(ctx->cpu.opcode)])
                ctx->cpu.pc += 2;
            ctx->cpu.pc += 2;
            break;
        case 0xA000:
            //(ANNN) Sets I to the address NNN
            ctx->cpu.I = GET_NNN(ctx->cpu.opcode);
            ctx->cpu.pc += 2;
            break;
        case 0xB000:
            //(BNNN) Jumps to the address NNN plus V0
            ctx->cpu.pc = GET_NNN(ctx->cpu.opcode) + (u_int16_t) ctx->cpu.V[0];
            break;
        case 0xC000:
            //(CXNN) Sets VX to the result of a bitwise and operation on a random number and NN
            srand(time(NULL));
            ctx->cpu.V[GET_X(ctx->cpu.opcode)] = (u_int8_t)(rand() % 256) & GET_NN(ctx->cpu.opcode);
            ctx->cpu.pc += 2;
            break;
        case 0xD000:
            //(DXYN) Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            ctx->cpu.V[0xF] = gpu_draw_sprit(ctx, ctx->cpu.V[GET_X(ctx->cpu.opcode)], ctx->cpu.V[GET_Y(ctx->cpu.opcode)], ctx->cpu.I, GET_N(ctx->cpu.opcode));
            ctx->cpu.pc += 2;
            break;
        case 0xE000:
            switch (ctx->cpu.opcode & 0x00FF)
            {
                case 0x009E:
                    //(EX9E) Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed
                    if (ctx->input.keys[ctx->cpu.V[GET_X(ctx->cpu.opcode)]])
                        ctx->cpu.pc += 2;
                    ctx->cpu.pc += 2;
                    break;
                case 0x00A1:
                    //(EXA1) Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed
                    if (!ctx->input.keys[ctx->cpu.V[GET_X(ctx->cpu.opcode)]])
                        ctx->cpu.pc += 2;
                    ctx->cpu.pc += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (ctx->cpu.opcode & 0x00FF)
            {
                case 0x0007:
                    //(FX07) Sets VX to the value of the delay timer
                    ctx->cpu.V[GET_X(ctx->cpu.opcode)] = ctx->cpu.dt;
                    ctx->cpu.pc += 2;
                    break;
                case 0x000A:
                    //(FX0A) A key press is awaited, and then stored in VX
                    for (u_int8_t i = 0; i < 16; i++){
                        if (ctx->input.keys[i]){
                            ctx->cpu.V[GET_X(ctx->cpu.opcode)] = i;
                            ctx->cpu.pc += 2;
                            break;
                        }
                    }
                    break;
                case 0x0015:
                    //(FX15) Sets the delay timer to VX
                    ctx->cpu.dt = ctx->cpu.V[GET_X(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0018:
                    //(FX18) Sets the sound timer to VX
                    ctx->cpu.st = ctx->cpu.V[GET_X(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x001E:
                    //(FX1E) Adds VX to I. VF is not affected
                    ctx->cpu.I += ctx->cpu.V[GET_X(ctx->cpu.opcode)];
                    ctx->cpu.pc += 2;
                    break;
                case 0x0029:
                    //(FX29) Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    ctx->cpu.I = ram_addr_font_sprit_in_mem(ctx->cpu.V[GET_X(ctx->cpu.opcode)] & 0x000F);
                    ctx->cpu.pc += 2;
                    break;
                case 0x0033:
                    //(FX33) Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
                    ram_write_mem(ctx, ctx->cpu.I, ((int)ctx->cpu.V[GET_X(ctx->cpu.opcode)]/100)%10);
                    ram_write_mem(ctx, ctx->cpu.I+1, ((int)ctx->cpu.V[GET_X(ctx->cpu.opcode)]/10)%10);
                    ram_write_mem(ctx, ctx->cpu.I+2, ((int)ctx->cpu.V[GET_X(ctx->cpu.opcode)]/1)%10);
                    ctx->cpu.pc += 2;
                    break;
                case 0x0055:
                    //(FX55) Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                    for (int i = 0; i < (int)(GET_X(ctx->cpu.opcode) + 1); i++)
                        ram_write_mem(ctx, ctx->cpu.I + i, ctx->cpu.V[i]);
                    ctx->cpu.pc += 2;
                    break;
                case 0x0065:
                    //(FX65) Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
                    for (int i = 0; i < (int)(GET_X(ctx->cpu.opcode) + 1); i++)
                        ctx->cpu.V[i] = ram_read_mem(ctx, ctx->cpu.I + i);
                    ctx->cpu.pc += 2;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void cpu_update_timers(Chip8_t* ctx){
    if (ctx->cpu.dt > 0)
        ctx->cpu.dt--;
    if (ctx->cpu.st > 0){
        ctx->cpu.st--;
    }
}
