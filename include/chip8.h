#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <SDL2/SDL.h>

typedef enum {
    CHIP8_RUNNING,
    CHIP8_PAUSED,
    CHIP8_STOPPED,
    CHIP8_DEBUG
} Chip8State_t;

typedef struct {
    u_int8_t** display;
    int update_display;
} Gpu_t;

typedef struct {
    u_int16_t* callstack;
    u_int8_t* V;
    u_int16_t I;
    u_int16_t sp;
    u_int16_t pc;
    u_int16_t dt;
    u_int16_t st;
    u_int16_t opcode;
} Cpu_t;

typedef struct {
    u_int8_t* memory;
} Ram_t;

typedef struct {
    u_int8_t * keys;
} Input_t;

typedef struct {
    Gpu_t gpu;
    Cpu_t cpu;
    Ram_t ram;
    Input_t input;
    Chip8State_t state;
    int intructions_per_second;
    int fps;
} Chip8_t;

#endif