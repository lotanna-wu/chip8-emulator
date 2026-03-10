#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INSTRUCTIONS_PER_FRAME 10

int g_running;

typedef struct {
    uint8_t  V[16];
    uint16_t I;
    uint16_t PC;
    uint8_t  SP;
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    uint16_t stack[16];
    uint8_t  memory[4096];
    uint32_t display[64 * 32];
    uint8_t  keys[16];
    uint8_t draw_flag;
} CHIP8;

uint16_t fetch(CHIP8* cpu);
void decode_exec(CHIP8* cpu, uint16_t opcode);

int load_rom(CHIP8* cpu, const char* path);
void chip8_init(CHIP8* cpu);
void update_timers(CHIP8* cpu);
void step(CHIP8* cpu);

