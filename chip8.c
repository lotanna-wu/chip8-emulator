#include "chip8.h"

void chip8_init(CHIP8* cpu) {
    cpu->PC          = 0x200;
    cpu->SP          = 0;
    cpu->I           = 0;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
    cpu->draw_flag   = 0;

    memset(cpu->V,       0, sizeof(cpu->V));
    memset(cpu->stack,   0, sizeof(cpu->stack));
    memset(cpu->memory,  0, sizeof(cpu->memory));
    memset(cpu->display, 0, sizeof(cpu->display));
    memset(cpu->keys,    0, sizeof(cpu->keys));

    uint8_t fonts[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&cpu->memory[0x000], fonts, sizeof(fonts));
}

int load_rom(CHIP8* cpu, const char* path) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        printf("No such file: %s\n", path);
        return 1;
    }

    size_t bytes_read = fread(&cpu->memory[0x200], 1, sizeof(cpu->memory) - 0x200, f);
    if (bytes_read == 0) {
        printf("Failed to read ROM: %s\n", path);
        fclose(f);
        return 1;
    }

    fclose(f);
    g_running = 1;
    return 0;
}

void step(CHIP8* cpu) {
    uint16_t opcode = fetch(cpu);

    decode(cpu, opcode);
    execute(cpu);
}

uint16_t fetch(CHIP8* cpu) {
    uint16_t opcode = ((uint16_t)cpu->memory[cpu->PC] << 8) | cpu->memory[cpu->PC + 1];
    cpu->PC += 2;
    return opcode;
}

void decode_exec(CHIP8* cpu, uint16_t opcode) {
    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;
    uint8_t  n   = (opcode & 0x000F);
    uint8_t  nn  = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch(opcode & 0xF000) {
        case 0x0000: {
            switch(opcode & 0x00FF) {
                case 0x00E0: // CLS
                case 0x00EE: // RET
            }
            break;
        }
        case 0x1000: {
            // JP nnn
            uint16_t addr = nnn;
            // ...
            break;
        }
        case 0x2000: {
            // CALL nnn
            uint16_t addr = nnn;
            // ...
            break;
        }
        case 0x3000: {
            // SE Vx, nn
            uint8_t Vx = cpu->V[x];
            if (Vx == nn) cpu-> PC += 2;
            break;
        }
        case 0x4000: {
            uint8_t Vx = cpu->V[x];
            if (Vx != nn) cpu-> PC += 2;
            break;
        }
        case 0x5000: {
            // SNE Vx, Vy
            uint8_t Vx = cpu->V[x];
            uint8_t Vy = cpu->V[y];
            if(Vx == Vy) cpu->PC += 2;
        }
        case 0x6000: {
            // LD Vx, nn
            cpu->V[x] = nn;   
            break;     
        }
        case 0x7000: {
            // ADD Vx, nn
            cpu->V[x] += nn;
            break;
        }
        case 0x8000: {
            switch(n) {
                case 0x000: {
                    // LD Vx, Vy
                    cpu->V[x] = cpu->V[y];
                    break;
                }
                case 0x001: {

                }
                case 0x002: {

                }
                case 0x003: {

                }
                case 0x004: {

                }
                case 0x005: {

                }
                case 0x006: {

                }
                case 0x007: {

                }
                case 0x00E: {

                }
                break;
            }
        }
        case 0x9000:
        case 0xA000:
        case 0xB000:
        case 0xC000:
        case 0xD000:
        case 0xE000:
        case 0xF000:
    }
}

void execute(CHIP8* cpu) {
}

void update_timers(CHIP8* cpu) {
    if (cpu->delay_timer > 0) cpu->delay_timer--;
    if (cpu->sound_timer > 0) cpu->sound_timer--;
}