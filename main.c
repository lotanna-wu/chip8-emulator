#include "chip8.h"
#include "display.h"
#include <stdio.h>

extern int g_running;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: chip8 <rom>\n");
        return 1;
    }

    CHIP8 cpu;
    chip8_init(&cpu);
    if(load_rom(&cpu, argv[1]) != 0) return 1;


    while (g_running) {
        for(int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            step(&cpu);
        }

        update_timers(&cpu);
        // SDL 
    }
}