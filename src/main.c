#include <stdio.h>
#include "chip8.h"
#include "display.h"
#include "input.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: chip8 <rom>\n");
        return 1;
    }

    CHIP8 cpu;
    int timer_accumulator = 0;
    chip8_init(&cpu);
    if(load_rom(&cpu, argv[1]) != 0) return 1;
    if(display_init() != 0) return 1;

    while (cpu.running) {
        handle_input(&cpu);
        step(&cpu);

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
