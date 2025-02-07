#include <cpu.h>
#include <memory.h>
#include <helper.h>
#include <graphic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GET_X(opcode) (((opcode) & 0x0F00) >> 8)
#define GET_Y(opcode) (((opcode) & 0x00F0) >> 4)
#define GET_N(opcode) ((opcode) & 0x000F)
#define GET_NN(opcode) ((u_int8_t)((opcode) & 0x00FF))
#define GET_NNN(opcode) ((opcode) & 0x0FFF)

static u_int8_t V[16];
static u_int16_t I;
static u_int16_t sp;
static u_int16_t pc;
static u_int16_t dt;
static u_int16_t st;

static u_int16_t callstack[16];

static u_int16_t opcode;

void fetch_decode_execute_instruction(){
    opcode = read_mem(pc) << 8 | read_mem(pc + 1);
    switch (opcode & 0xF000){
        case 0x0000:
            switch (opcode & 0x0FFF){
                case 0x00E0:
                    //(00E0) Clears the screen
                    clear_display();
                    pc += 2;
                    break;
                case 0x00EE:
                    //(00EE) Returns from a subroutine
                    pc = callstack[--sp];
                    pc += 2;
                    break;
                default: 
                    //TODO: (0NNN) Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN
                    pc += 2;
                    break;
            }
            break;
        case 0x1000:
            //(1NNN) Jumps to address NNN
            pc = GET_NNN(opcode);
            break;
        case 0x2000:
            //(2NNN) Calls subroutine at NNN
            callstack[sp++] = pc;
            pc = GET_NNN(opcode);
            break;
        case 0x3000:
            //(3XNN) Skips the next instruction if VX equals NN
            pc += 2;
            if (V[GET_X(opcode)] == GET_NN(opcode))
                pc += 2;
            break;
        case 0x4000:
            //(4XNN) Skips the next instruction if VX does not equal NN
            pc += 2;
            if (V[GET_X(opcode)] != GET_NN(opcode))
                pc += 2;
            break;
        case 0x5000:
            //(5XY0) Skips the next instruction if VX equals VY
            pc += 2;
            if (V[GET_X(opcode)] == V[GET_Y(opcode)])
                pc += 2;
            break;
        case 0x6000:
            //(6XNN) Sets VX to NN
            V[GET_X(opcode)] = GET_NN(opcode);
            pc += 2;
            break;
        case 0x7000:
            //(7XNN) Adds NN to VX (carry flag is not changed)
            V[GET_X(opcode)] += GET_NN(opcode);
            pc += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F){
                case 0x0000:
                    //(8XY0) Sets VX to the value of VY
                    V[GET_X(opcode)] = V[GET_Y(opcode)];
                    pc += 2;
                    break;
                case 0x0001:
                    //(8XY1) Sets VX to VX or VY
                    V[GET_X(opcode)] |= V[GET_Y(opcode)];
                    pc += 2;
                    break;
                case 0x0002:
                    //(8XY2) Sets VX to VX and VY
                    V[GET_X(opcode)] &= V[GET_Y(opcode)];
                    pc += 2;
                    break;
                case 0x0003:
                    //(8XY3) Sets VX to VX xor VY
                    V[GET_X(opcode)] ^= V[GET_Y(opcode)];
                    pc += 2;
                    break;
                case 0x0004:
                    //(8XY4) Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
                    V[0xF] = (u_int8_t) ((((u_int16_t) V[GET_X(opcode)] + (u_int16_t) V[GET_Y(opcode)]) & 0xF00) >> 8); //TODO: Review overflow bit
                    V[GET_X(opcode)] += V[GET_Y(opcode)];
                    pc += 2;
                    break;
                case 0x0005:
                    //(8XY5) VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    V[GET_X(opcode)] -= V[GET_Y(opcode)];
                    V[0xF] = V[GET_X(opcode)] < V[GET_Y(opcode)] ? 0 : 1;
                    pc += 2;
                    break;
                case 0x0006:
                    //(8XY6) Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
                    V[GET_X(opcode)] >>= 1;
                    V[0xF] = V[GET_X(opcode)] & 0b00000001;
                    pc += 2;
                    break;
                case 0x0007:
                    //(8XY7) Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    V[GET_X(opcode)] = V[GET_Y(opcode)] - V[GET_X(opcode)];
                    V[0xF] = V[GET_Y(opcode)] < V[GET_X(opcode)] ? 0 : 1;
                    pc += 2;
                    break;
                case 0x000E:
                    //(8XYE) Skips the next instruction if VX does not equal VY
                    pc += 2;
                    if (V[GET_X(opcode)] != V[GET_Y(opcode)])
                        pc += 2;
                    break;
                default: 
                    break;
            }
            break;
        case 0xA000:
            //(ANNN) Sets I to the address NNN
            I = GET_NNN(opcode);
            pc += 2;
            break;
        case 0xB000:
            //(BNNN) Jumps to the address NNN plus V0
            pc = GET_NNN(opcode) + (u_int16_t) V[0];
            break;
        case 0xC000:
            //(CXNN) Sets VX to the result of a bitwise and operation on a random number and NN
            srand(time(NULL));
            V[GET_X(opcode)] = (u_int8_t)(rand() % 256) & GET_NN(opcode);
            pc += 2;
            break;
        case 0xD000:
            //(DXYN) Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            V[0xF] = draw_sprit(V[GET_X(opcode)], V[GET_Y(opcode)], I, GET_N(opcode));
            system("clear");
            print_display();
            pc += 2;
            break;
        case 0xE000:
            switch (opcode & 0x00FF)
            {
                case 0x009E:
                    //TODO: (EX9E) Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed
                    pc += 2;
                    break;
                case 0x00A1:
                    //TODO: (EXA1) Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed
                    pc += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007:
                    //(FX07) Sets VX to the value of the delay timer
                    V[GET_X(opcode)] = dt;
                    pc += 2;
                    break;
                case 0x000A:
                    //TODO: (FX0A) A key press is awaited, and then stored in VX
                    pc += 2;
                    break;
                case 0x0015:
                    //(FX15) Sets the delay timer to VX
                    dt = V[GET_X(opcode)];
                    pc += 2;
                    break;
                case 0x0018:
                    //(FX18) Sets the sound timer to VX
                    st = V[GET_X(opcode)];
                    pc += 2;
                    break;
                case 0x001E:
                    //(FX1E) Adds VX to I. VF is not affected
                    I += V[GET_X(opcode)];
                    pc += 2;
                    break;
                case 0x0029:
                    //(FX29) Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    I = addr_font_sprit_in_mem(GET_X(opcode) & 0x000F);
                    pc += 2;
                    break;
                case 0x0033:
                    //TODO: (FX33) Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
                    pc += 2;
                    break;
                case 0x0055:
                    //(FX55) Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                    for (int i = 0; i < (int)(GET_X(opcode) + 1); i++)
                        write_mem(I + i, V[i]);
                    pc += 2;
                    break;
                case 0x0065:
                    //(FX65) Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
                    for (int i = 0; i < (int)(GET_X(opcode) + 1); i++)
                        V[i] = read_mem(I + i);
                    pc += 2;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void update_timers(){
    if (dt > 0)
        dt--;
    if (st > 0){
        putchar('\a');
        st--;
    }
}

void init_cpu(){
    I = 0;
    sp = 0;
    dt = 0;
    st = 0;
    pc = MEM_ADDR_START_PROG;
    for (int i = 0; i < 16; i++)
        V[i] = 0;
}

void loop_cpu(){
    fetch_decode_execute_instruction();
    update_timers();
}

