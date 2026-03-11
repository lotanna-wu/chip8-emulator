#include "input.h"

int map_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;
        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;
        default:     return -1;
    }
}

void handle_input(CHIP8* cpu) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                cpu->running = 0;
                break;
            case SDL_KEYDOWN: {
                int key = map_key(e.key.keysym.sym);
                if (key != -1) cpu->keys[key] = 1;
                break;
            }
            case SDL_KEYUP: {
                int key = map_key(e.key.keysym.sym);
                if (key != -1) cpu->keys[key] = 0;
                break;
            }
        }
    }
}