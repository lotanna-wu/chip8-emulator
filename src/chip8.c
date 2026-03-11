#include "chip8.h"

void chip8_init(CHIP8* cpu) {
    srand(time(NULL));

    cpu->PC          = 0x200;
    cpu->SP          = 0;
    cpu->I           = 0;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
    cpu->draw_flag   = 0;
    cpu->running     = 0;

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
    cpu->running = 1;
    return 0;
}

void step(CHIP8* cpu) {
    uint16_t opcode = fetch(cpu);

    decode_exec(cpu, opcode);
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

    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (nn) {
                case 0xE0: {
                    // CLS
                    for (int i = 0; i < 64 * 32; i++)
                        cpu->display[i] = PIXEL_OFF;
                    cpu->draw_flag = 1;
                    break;
                }
                case 0xEE: {
                    // RET
                    cpu->SP--;
                    cpu->PC = cpu->stack[cpu->SP];
                    break;
                }
            }
            break;
        }
        case 0x1000: { 
            // JP nnn
            cpu->PC = nnn;
            break;
        }
        case 0x2000: { 
            // CALL nnn
            if (cpu->SP >= 16) {
                // stack overflow
                cpu->running = 0;
                break;
            }
            cpu->stack[cpu->SP] = cpu->PC;
            cpu->SP++;
            cpu->PC = nnn;
            break;
        }
        case 0x3000: { 
            // SE Vx, nn
            if (cpu->V[x] == nn) cpu->PC += 2;
            break;
        }
        case 0x4000: { 
            // SNE Vx, nn
            if (cpu->V[x] != nn) cpu->PC += 2;
            break;
        }
        case 0x5000: { 
            // SE Vx, Vy
            if (cpu->V[x] == cpu->V[y]) cpu->PC += 2;
            break;
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
            switch (n) {
                case 0x0: { 
                    // LD Vx, Vy
                    cpu->V[x] = cpu->V[y];
                    break;
                }
                case 0x1: { 
                    // OR Vx, Vy
                    cpu->V[x] |= cpu->V[y];
                    break;
                }
                case 0x2: { 
                    // AND Vx, Vy
                    cpu->V[x] &= cpu->V[y];
                    break;
                }
                case 0x3: { 
                    // XOR Vx, Vy
                    cpu->V[x] ^= cpu->V[y];
                    break;
                }
                case 0x4: { 
                    // ADD Vx, Vy
                    uint8_t Vx    = cpu->V[x];
                    uint8_t sum   = Vx + cpu->V[y];
                    uint8_t carry = (sum < Vx) ? 1 : 0;
                    cpu->V[x]     = sum;
                    cpu->V[0xF]   = carry;
                    break;
                }
                case 0x5: { 
                    // SUB Vx, Vy
                    uint8_t Vx         = cpu->V[x];
                    uint8_t Vy         = cpu->V[y];
                    uint8_t did_borrow = (Vx <= Vy) ? 1 : 0;
                    cpu->V[x]          = Vx - Vy;
                    cpu->V[0xF]        = !did_borrow;
                    break;
                }
                case 0x6: { 
                    // SHR Vx {, Vy}
                    uint8_t Vx  = cpu->V[x];
                    cpu->V[x]   = Vx >> 1;
                    cpu->V[0xF] = Vx & 0x1;
                    break;
                }
                case 0x7: {
                    // SUBN Vx, Vy
                    uint8_t Vx         = cpu->V[x];
                    uint8_t Vy         = cpu->V[y];
                    uint8_t did_borrow = (Vy <= Vx) ? 1 : 0;
                    cpu->V[x]          = Vy - Vx;
                    cpu->V[0xF]        = !did_borrow;
                    break;
                }
                case 0xE: {
                    // SHL Vx {, Vy}
                    uint8_t Vx  = cpu->V[x];
                    cpu->V[x]   = Vx << 1;
                    cpu->V[0xF] = (Vx >> 7) & 0x1;
                    break;
                }
            }
            break;
        }
        case 0x9000: {
            uint8_t Vx =  cpu->V[x];
            uint8_t Vy =  cpu->V[y];
            if (Vx != Vy) cpu->PC += 2;
            break;
        }
        case 0xA000: {
            //LD I, nnn
            cpu->I = nnn;
            break;
        }
        case 0xB000: {
            // JP V0, addr
            cpu->PC = cpu->V[0] + nnn;
            break;
        }
        case 0xC000: {
            // RND Vx, nn
            cpu->V[x] = (rand() % 256) & nn;
            break;
        }
        case 0xD000: {
            // DRW Vx, Vy, n
            uint8_t xpos = cpu->V[x] % CHIP8_WIDTH;
            uint8_t ypos = cpu->V[y] % CHIP8_HEIGHT;
            cpu->V[REG_VF] = 0;

            for (int r = 0; r < n; r++) {
                uint8_t sprite = cpu->memory[cpu->I + r];

                for (int c = 0; c < 8; c++) {
                    if (sprite & (0x80 >> c)) {
                        int px = (xpos + c) % 64;
                        int py = (ypos + r) % 32;
                        int idx = px + (CHIP8_WIDTH * py);

                        if (cpu->display[idx]) cpu->V[0xF] = 1;
                        cpu->display[idx] ^= PIXEL_ON;

                    } 
                }
            }
            cpu->draw_flag = 1;
            break;
        }
        case 0xE000: {
            switch(nn) {
                case 0x9E: {
                    // SKP Vx
                    if (cpu->keys[cpu->V[x] & 0xF]) cpu->PC += 2;
                    break;
                }
                case 0xA1: {
                    // SKNP Vx
                    if (!cpu->keys[cpu->V[x] & 0xF]) cpu->PC += 2;
                    break;
                }
            }
            break;
        }
        case 0xF000: {
            switch(nn) {
                case 0x07: {
                    // LD Vx, DT
                    cpu->V[x] = cpu->delay_timer;
                    break;
                }
                case 0x0A: {
                    // LD Vx, K
                    // For now im going to implement this by rewinding this instruction until key is pressed
                    for (int i = 0; i < 16; i++) {
                        if (cpu->keys[i]) {
                            cpu->V[x] = i;
                            return;
                        }
                    }
                    cpu->PC -= 2;
                    break;
                }
                case 0x15: {
                    // LD DT, Vx
                    cpu->delay_timer = cpu->V[x];
                    break;
                }
                case 0x18: {
                    // LD ST, Vx
                    cpu->sound_timer = cpu->V[x];
                    break;
                }
                case 0x1E: {
                    // ADD I, Vx
                    cpu->I += cpu->V[x];
                    break;
                }
                case 0x29: {
                    // LD F, Vx
                    if (cpu->V[x] > 0xF) break; 
                    cpu->I = cpu->V[x] * 5;
                    break;
                }
                case 0x33: {
                    // LD B, Vx
                    uint8_t Vx = cpu->V[x];
                    cpu->memory[cpu->I]     = Vx / 100;
                    cpu->memory[cpu->I + 1] = (Vx / 10) % 10;
                    cpu->memory[cpu->I + 2] = Vx % 10;
                    break;             
                }
                case 0x55: {
                    // LD [I], Vx
                    for (int i = 0; i <= x; i++) {
                        cpu->memory[cpu->I + i] = cpu->V[i];
                    }
                    break;
                }
                case 0x65: {
                    // LD Vx, [I]
                    for (int i = 0; i <= x; i++) {
                        cpu->V[i] = cpu->memory[cpu->I + i];
                    }
                    break;
                }
            }
            break;
        }
    }
}

void update_timers(CHIP8* cpu) {
    if (cpu->delay_timer > 0) cpu->delay_timer--;
    if (cpu->sound_timer > 0) cpu->sound_timer--;
}


// debugging

void dump_state(CHIP8* cpu) {
    printf("=== chip8 dump ===\n");

    printf("Registers:\n");
    for (int i = 0; i < 16; i++)
        printf("  V%X: 0x%02X (%d)\n", i, cpu->V[i], cpu->V[i]);

    printf("\nSpecial Registers:\n");
    printf("  PC: 0x%03X\n", cpu->PC);
    printf("  I:  0x%03X <- mem[I]: 0x%02X\n", cpu->I, cpu->memory[cpu->I]);
    printf("  SP: %d\n", cpu->SP);

    printf("\nTimers:\n");
    printf("  Delay: %d\n", cpu->delay_timer);
    printf("  Sound: %d\n", cpu->sound_timer);

    printf("\nStack:\n");
    for (int i = 0; i < cpu->SP; i++)
        printf("  [%d]: 0x%03X%s\n", i, cpu->stack[i], i == cpu->SP - 1 ? " <- SP" : "");

    printf("\nKeys:\n");
    for (int i = 0; i < 16; i++)
        printf("  %X:%c ", i, cpu->keys[i] ? '#' : '.');
    printf("\n");

    printf("\nNext opcode: 0x%02X%02X\n",
        cpu->memory[cpu->PC],
        cpu->memory[cpu->PC + 1]);

    printf("====================\n");
}
