#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define CHIP8_WIDTH   64
#define CHIP8_HEIGHT  32
#define SCALE         10
#define PIXEL_ON      0xFFFFFFFF
#define PIXEL_OFF     0xFF000000
#define SCREEN_WIDTH  (CHIP8_WIDTH  * SCALE)
#define SCREEN_HEIGHT (CHIP8_HEIGHT * SCALE)

int display_init();
void display_cleanup();
void display_draw(uint32_t* pixels);

#endif