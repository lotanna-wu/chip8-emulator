#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 320

int display_init();
void render(CHIP8* cpu);
void cleanup();
#endif