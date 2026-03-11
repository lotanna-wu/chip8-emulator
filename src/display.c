#include "display.h"

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;

int display_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL couldn't initialize.\nSDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Chip 8 Emulator", 
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window couldn't be created.\n"
               "SDL_Error: %s\n", SDL_GetError());
        goto cleanup; 
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer couldn't be created.\n"
            "SDL_Error: %s\n", SDL_GetError());
        goto cleanup; 
    }

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                CHIP8_WIDTH, CHIP8_HEIGHT);

    if (!texture) {
        printf("Texture couldn't be created.\n"
            "SDL_Error: %s\n", SDL_GetError());
        goto cleanup;
    }
    return 0;
    
cleanup:
    if (texture)  SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    return -1;
}


void display_cleanup() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void display_draw(uint32_t* pixels) {
    SDL_UpdateTexture(texture, NULL, pixels, CHIP8_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}