#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCALE 10

void todo(void)
{
    return;
}

int main(void)
{
    // Memory and stack
    uint8_t mem[4096];
    uint16_t stack[16];
    bool display[2048];

    // Program counter and stack pointer
    uint16_t pc = 0x200;
    uint8_t sp = 0;

    // Data registers
    uint8_t v[16];

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
  
    SDL_Window *win;
    SDL_Renderer *ren;

    int width = 64 * SCALE;
    int height = 32 * SCALE;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren);
    SDL_RenderSetScale(ren, SCALE, SCALE);
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
    
    while (true)
    {
        if (SDL_PollEvent(&e) && e.type == SDL_QUIT)
            break;
        
        uint16_t opcode = (mem[pc] << 8) | mem[pc + 1];
        printf("OPCODE = %04X\n", opcode);

        // Test the first hexadecimal digit of the opcode
        switch (opcode & 0xF000)
        {
            case 0x0:
                switch (opcode)
                {
                    // Clear the screen
                    case 0x00E0:
                        for (size_t i = 0; i < 2048; i++)
                            display[i] = false;
                        break;
                    
                    case 0x00EE:
                        // TODO: Return from a subroutine
                        todo();
                        break;
                    
                    default:
                        break;
                }
                break;
            
            case 0x1:
                pc = opcode & 0x0FFF;
                break;
            
            case 0x2:
                // Execute subroutine starting at address NNN
                todo();
                break;
            
            case 0x3:
                if (v[opcode & 0x0F00] == (opcode & 0x00FF))
                    pc += 4;
                else
                    pc += 2;
                break;

            case 0x4:
                if (v[opcode & 0x0F00] != (opcode & 0x00FF))
                    pc += 4;
                else
                    pc += 2;
                break;
            
            case 0x5:
                if (v[opcode & 0x0F00] == v[opcode & 0x00F0])
                    pc += 4;
                else
                    pc += 2;
                break;

            case 0x6:
                v[opcode & 0x0F00] = opcode & 0x00FF;
                pc += 2;
                break;
            
            case 0x7:
                v[opcode & 0x0F00] += opcode & 0x00FF;
                pc += 2;
                break;

            case 0x8:
                switch (opcode & 0x000F)
                {
                    case 0x0:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0];
                        pc += 2;
                        break;
                    
                    case 0x1:
                        v[opcode & 0x0F00] |= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    case 0x2:
                        v[opcode & 0x0F00] &= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    case 0x3:
                        v[opcode & 0x0F00] ^= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    case 0x4:
                        if ((uint64_t) (v[opcode & 0x0F00]) + (uint64_t) (v[opcode & 0x00F0]) > UINT8_MAX)
                            v[0xF] = 1;
                        else
                            v[0xF] = 0;
                        v[opcode & 0x0F00] += v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    case 0x5:
                        //Subtract value of VY from VX
                        // Set VF to 0 if a borrow occurs, set to 1 otherwise
                        todo();
                        break;
                    
                    case 0x6:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0] >> 1;
                        v[0xF] = v[opcode & 0x00F0] & 0xF;
                        break;
                    
                    case 0x7:
                        // Set VX to the value of VY minus VX
                        // Set VF to 0 if a borrow occurs, set to 1 otherwise
                        todo();
                        break;
                    
                    case 0xE:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0] << 1;
                        v[0xF] = v[opcode & 0x0F00] & 0xF;
                        break;

                    default:
                        break;
                }
                break;
            
            case 0x9:
                if (v[opcode & 0x0F00] != v[opcode & 0x00F0])
                    pc += 4;
                else
                    pc += 2;
                break;
            
            case 0xA:
                I = opcode & 0x0FFF;
                pc += 2;
                break;

            case 0xB:
                pc = v[0] + (opcode & 0x0FFF);
                break;

            case 0xC:
                v[opcode & 0x0F00] = 23; // random !
                break;
            
            case 0xD:
                // Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I. 
                // Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
                todo();
                break;

            case 0xE:
                switch (opcode & 0x00FF)
                {
                case 0x9E:
                    //if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == )
                    todo();
                    break;

                case 0xA1:
                    todo();
                    break;
                
                default:
                    break;
                }
                break;
            
            case 0xF:
                switch (opcode & 0x00FF)
                {
                    case 0x07:
                        todo();
                        break;
                    
                    case 0x0A:
                        todo();
                        break;

                    case 0x15:
                        todo();
                        break;
                    
                    case 0x18:
                        todo();
                        break;
                    
                    case 0x1E:
                        todo();
                        break;

                    case 0x29:
                        todo();
                        break;

                    case 0x33:
                        todo();
                        break;

                    case 0x55:
                        todo();
                        break;

                    case 0x65:
                        todo();
                        break;

                    default:
                        break;
                }
                break;
            
            default:
                printf("Unrecognized opcode %04X. Skipping.\n", opcode);
                break;
        }

        // Redraw what's on screen
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (display[i * height + j])
                    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_RenderDrawPoint(ren, i, j);
            }
        }
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}