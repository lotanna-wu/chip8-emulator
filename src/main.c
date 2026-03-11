#include <stdio.h>
#include <string.h>
#include "chip8.h"
#include "display.h"
#include "input.h"

int main(int argc, char** argv) {
    int verbose = 0;
    const char* rom_path = NULL;

    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        if (arg == NULL) continue;
        if (strcmp(arg, "--") == 0) {
            if (i + 1 < argc) rom_path = argv[i + 1];
            break;
        }
        if (arg[0] == '-') {
            if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
                verbose = 1;
                continue;
            }
            if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
                printf("Usage: %s [-v|--verbose] <rom>\n", argv[0]);
                return 1;
            }
            printf("Usage: %s [-v|--verbose] <rom>\n", argv[0]);
            return 1;
        }
        if (rom_path == NULL) {
            rom_path = arg;
            continue;
        }
        printf("Usage: %s [-v|--verbose] <rom>\n", argv[0]);
        return 1;
    }

    if (rom_path == NULL) {
        printf("Usage: %s [-v|--verbose] <rom>\n", argv[0]);
        return 1;
    }

    CHIP8 cpu;
    int timer_accumulator = 0;
    chip8_init(&cpu);

    if(load_rom(&cpu, rom_path) != 0) return 1;
    if(display_init() != 0) return 1;

    while (cpu.running) {
        handle_input(&cpu);
        step(&cpu);
        if (verbose) dump_state(&cpu);
        
        // decrement timers at 60Hz
        timer_accumulator++;
        if (timer_accumulator >= CPU_HZ / TIMER_HZ) {
            update_timers(&cpu);
            timer_accumulator = 0;
        }

        if (cpu.draw_flag) {
            display_draw(cpu.display);
            cpu.draw_flag = 0;
        }

        SDL_Delay(MS_PER_INSTR);
    }

    display_cleanup();
}
