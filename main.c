#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

int main(void)
{
    // Memory and stack
    uint8_t mem[4096];
    uint16_t stack[16];

    // Program counter and stack pointer
    uint16_t PC = 0x200;
    uint8_t SP = 0;

    // Data registers
    uint8_t V0;
    uint8_t V1;
    uint8_t V2;
    uint8_t V3;
    uint8_t V4;
    uint8_t V5;
    uint8_t V6;
    uint8_t V7;
    uint8_t V8;
    uint8_t V9;
    uint8_t VA;
    uint8_t VB;
    uint8_t VC;
    uint8_t VD;
    uint8_t VE;
    uint8_t VF;

    // Address register
    uint16_t I;

    // Delay timer and sound timer
    uint8_t dt;
    uint8_t st;

    // Storing font data (one line per char, 0 -> F) into reserved memory space (0x000 -> 0x04F included)
    uint8_t *font_byte = mem;
    *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0x90; *font_byte++ = 0x90; *font_byte++ = 0xF0;
    *font_byte++ = 0x20; *font_byte++ = 0x60; *font_byte++ = 0x20; *font_byte++ = 0x20; *font_byte++ = 0x70;
    *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0;
    *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0xF0;
    *font_byte++ = 0x90; *font_byte++ = 0x90; *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0x10;
    *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0xF0;
    *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0xF0;
    *font_byte++ = 0xF0; *font_byte++ = 0x10; *font_byte++ = 0x20; *font_byte++ = 0x40; *font_byte++ = 0x40;
    *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0xF0;
    *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0x90;
    *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0xF0; *font_byte++ = 0x90; *font_byte++ = 0x90;
    *font_byte++ = 0xE0; *font_byte++ = 0x90; *font_byte++ = 0xE0; *font_byte++ = 0x90; *font_byte++ = 0xE0;
    *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0x80; *font_byte++ = 0x80; *font_byte++ = 0xF0;
    *font_byte++ = 0xE0; *font_byte++ = 0x90; *font_byte++ = 0x90; *font_byte++ = 0x90; *font_byte++ = 0xE0;
    *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0;
    *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte++ = 0xF0; *font_byte++ = 0x80; *font_byte = 0x80;

    // for (;;)
    // {

    //     switch (opcode)
    //     {
            
    //     }
    // }

    

    SDL_Window *win;
    SDL_Renderer *ren;

    int scale = 10;
    int width = 64 * scale;
    int height = 32 * scale;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren);
    SDL_RenderSetScale(ren, scale, scale);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            SDL_RenderDrawPoint(ren, i, j);
        }
    }
    SDL_RenderPresent(ren);
    
    SDL_Event e;
    while (1)
    {
        if (SDL_PollEvent(&e) && e.type == SDL_QUIT)
            break;
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}