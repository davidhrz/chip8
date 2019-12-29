#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCALE 10
#define WIDTH 64
#define HEIGHT 32

void draw_pixel_inarray(bool *display, size_t x, size_t y, bool state)
{
    *(display + (y * WIDTH + x)) = state;
}

void todo(uint8_t opcode)
{
    printf("Opcode %04X not implemented.\n", opcode);
    return;
}

int main(int argc, char **argv)
{
    // 4096 B memory
    uint8_t mem[4096];
    for (size_t i = 0; i < 4096; i++)
        mem[i] = 0;

    // Load ROM file into memory
    if (argc > 1)
    {
        FILE *rom = fopen(argv[1], "rb");

        // Read file size
        fseek(rom, 0, SEEK_END);
        long fsize = ftell(rom);
        fseek(rom, 0, SEEK_SET);

        // Read file to memory starting at address 0x200
        fread(mem + 0x200, sizeof(uint8_t), fsize, rom);
    }

    uint16_t stack[16];
    
    bool display[2048];
    for (size_t i = 0; i < 2048; i++)
        display[i] = false;

    // Program counter and stack pointer
    uint16_t pc = 0x200;
    uint8_t sp = 0;

    // Data registers
    uint8_t v[16];
    for (size_t i = 0; i < 16; i++)
        v[i] = 0;

    // Address register
    uint16_t I = 0;

    // Delay timer and sound timer
    uint8_t dt = 0;
    uint8_t st = 0;

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

    // MEM DUMP
    // for (size_t i = 0; i < 4096; i++)
    //     printf("%03X : %02X\n", i, mem[i]);

    // Initialize SDL
    SDL_Window *win;
    SDL_Renderer *ren;

    int width = WIDTH * SCALE;
    int height = HEIGHT * SCALE;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren);
    SDL_RenderSetScale(ren, SCALE, SCALE);
    
    // Clear screen
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            SDL_RenderDrawPoint(ren, j, i);
        }
    }
    SDL_RenderPresent(ren);
    
    SDL_Event e;
    while (true)
    {
        if (pc > 4095)
        {
            printf("Reached end of memory\n");
            break;
        }
        if (SDL_PollEvent(&e) && e.type == SDL_QUIT)
            break;
        
        uint16_t opcode = (mem[pc] << 8) | mem[pc + 1];
        printf("OPCODE = %04X\n", opcode);

        // Test the first hexadecimal digit of the opcode
        switch (opcode & 0xF000)
        {
            case 0x0000:
                switch (opcode)
                {
                    // Clear the screen
                    case 0x00E0:
                        for (size_t i = 0; i < 2048; i++)
                            display[i] = false;
                        pc += 2;
                        break;
                    
                    // Return from subroutine
                    case 0x00EE:
                        pc = stack[sp--];
                        break;
                    
                    default:
                        break;
                }
                break;
            
            // Jump to address
            case 0x1000:
                pc = opcode & 0x0FFF;
                break;
            
            // Execute subroutine starting at address
            case 0x2000:
                stack[++sp] = pc;
                pc = opcode & 0x0FFF;
                break;
            
            // Skip following instruction if VX == NN
            case 0x3000:
                if (v[opcode & 0x0F00] == (opcode & 0x00FF))
                    pc += 4;
                else
                    pc += 2;
                break;

            // Skip following instruction if VX != NN
            case 0x4000:
                if (v[opcode & 0x0F00] != (opcode & 0x00FF))
                    pc += 4;
                else
                    pc += 2;
                break;
            
            // Skip following instruction if VX == VY
            case 0x5000:
                if (v[opcode & 0x0F00] == v[opcode & 0x00F0])
                    pc += 4;
                else
                    pc += 2;
                break;

            // Store NN in VX
            case 0x6000:
                v[opcode & 0x0F00] = opcode & 0x00FF;
                pc += 2;
                break;
            
            // Add NN to VX
            case 0x7000:
                v[opcode & 0x0F00] += opcode & 0x00FF;
                pc += 2;
                break;

            case 0x8000:
                switch (opcode & 0x000F)
                {
                    // Store VY in VX
                    case 0x0:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0];
                        pc += 2;
                        break;
                    
                    // Set VX to VX | VY
                    case 0x1:
                        v[opcode & 0x0F00] |= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    // Set VX to VX && VY
                    case 0x2:
                        v[opcode & 0x0F00] &= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    // Set VX to VX ^ VY
                    case 0x3:
                        v[opcode & 0x0F00] ^= v[opcode & 0x00F0];
                        pc += 2;
                        break;

                    // Add VY to VX, set VF if carry occurs
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
                        todo(opcode);
                        break;
                    
                    // Set VX to (VY >> 1), set VF to LSB prior to the shift
                    case 0x6:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0] >> 1;
                        v[0xF] = v[opcode & 0x00F0] & 0xF;
                        pc += 2;
                        break;
                    
                    case 0x7:
                        // Set VX to the value of VY minus VX
                        // Set VF to 0 if a borrow occurs, set to 1 otherwise
                        todo(opcode);
                        break;
                    
                    // Set VX to (VX << 1), set VF to MSB prior to the shift
                    case 0xE:
                        v[opcode & 0x0F00] = v[opcode & 0x00F0] << 1;
                        v[0xF] = v[opcode & 0x0F00] & 0xF;
                        pc += 2;
                        break;

                    default:
                        break;
                }
                break;
            
            // Skip next instruction if VX != VY
            case 0x9000:
                if (v[opcode & 0x0F00] != v[opcode & 0x00F0])
                    pc += 4;
                else
                    pc += 2;
                break;
            
            // Store NNN in I
            case 0xA000:
                I = opcode & 0x0FFF;
                pc += 2;
                break;
            
            // Jump to NNN + V0
            case 0xB000:
                pc = v[0] + (opcode & 0x0FFF);
                break;

            // Set VX to random number
            case 0xC000:
                v[opcode & 0x0F00] = 23; // random !
                break;
            
            case 0xD000:
                // Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I. 
                // Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
                // X = v[opcode & 0x0F00]
                // Y = v[opcode & 0x00F0]
                // N = v[opcode & 0x000F]
                for (uint8_t y = 0; y < v[opcode & 0x000F]; y++)
                {
                    for (uint8_t x = 0; x < 8; x++)
                    {
                        display[v[opcode & 0x00F0] * WIDTH + v[opcode & 0x0F00]] = display[v[opcode & 0x00F0] * WIDTH + v[opcode & 0x0F00]] ^ mem[I + y];
                    }
                }
                pc += 2;
                break;

            case 0xE000:
                switch (opcode & 0x00FF)
                {
                    case 0x9E:
                        //if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == )
                        todo(opcode);
                        break;

                    case 0xA1:
                        todo(opcode);
                        break;
                    
                    default:
                        break;
                }
                break;
            
            case 0xF000:
                switch (opcode & 0x00FF)
                {
                    case 0x07:
                        v[opcode & 0x0F00] = dt;
                        pc += 2;
                        break;
                    
                    case 0x0A:
                        // Wait for a keypress and store result in VX
                        todo(opcode);
                        break;

                    case 0x15:
                        dt = v[opcode & 0x0F00];
                        pc += 2;
                        break;
                    
                    case 0x18:
                        st = v[opcode & 0x0F00];
                        pc += 2;
                        break;
                    
                    case 0x1E:
                        I += v[opcode & 0x0F00];
                        pc += 2;
                        break;

                    case 0x29:
                        I = v[opcode & 0x0F00] * 5;
                        pc += 2;
                        break;

                    case 0x33:
                        mem[I] = v[(opcode & 0x0F00) >> 8] / 100;
                        mem[I + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
                        mem[I + 2] = (v[(opcode & 0x0F00) >> 8] % 100) % 10;
                        pc += 2;
                        break;

                    case 0x55:
                        for (size_t i = 0; i <= (opcode & 0x0F00); i++)
                        {
                            mem[I++] = v[i];
                        }
                        pc += 2;
                        break;

                    case 0x65:
                        for (size_t i = 0; i <= (opcode & 0x0F00); i++)
                        {
                            v[i] = mem[I++];
                        }
                        pc += 2;
                        break;

                    default:
                        break;
                }
                break;
            
            default:
                printf("Unrecognized opcode %04X. Skipping.\n", opcode);
                break;
        }
        // Redraw display
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                // printf("I = %i, J = %i\n", i, j);
                if (display[i * WIDTH + j] == true)
                    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_RenderDrawPoint(ren, j, i);
            }
        }
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}