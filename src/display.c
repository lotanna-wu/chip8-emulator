#include "display.h"

static SDL_Window* window;
static SDL_Renderer* renderer;

int display_init() {
    window = SDL_CreateWindow("Chip 8 Emulator", 
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window couldn't be created.\n"
               "SDL_Error: %s\n", SDL_GetError());
        return -1; 
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer couldn't be created.\n"
            "SDL_Error: %s\n", SDL_GetError());
        return -1; 
    }
}