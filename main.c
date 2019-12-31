#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
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
    // Seed random generator with current time
    srand(time(NULL));

    // Array of 4096 bytes to represent memory
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

    // Array of 16 16-bit values to represent the stack
    uint16_t stack[16];

    // Data registers
    uint8_t v[16];
    for (size_t i = 0; i < 16; i++)
        v[i] = 0;

    // Program counter and stack pointer
    uint16_t pc = 0x200;
    uint8_t sp = 0;

    // Address register
    uint16_t I = 0;

    // Delay timer and sound timer
    uint8_t dt = 0;
    uint8_t st = 0;

    // Array of 64 * 32 booleans to represent display
    // false = pixel off, true = pixel on
    bool display[2048];
    for (size_t i = 0; i < 2048; i++)
        display[i] = false;

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
    bool quit = false;

    int width = WIDTH * SCALE;
    int height = HEIGHT * SCALE;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren);
    SDL_SetWindowTitle(win, "Chip-8");
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
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                
                case SDL_KEYDOWN:
                    printf("Pressed: %s\n", SDL_GetKeyName(e.key.keysym.sym));
                    break;

                case SDL_KEYUP:
                    printf("Released: %s\n", SDL_GetKeyName(e.key.keysym.sym));

                default:
                    break;
            }
        }
        if (quit)
            break;
        
        uint16_t prev_opcode = 0;
        uint16_t opcode = (mem[pc] << 8) | mem[pc + 1];
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;
        uint16_t nnn = opcode & 0x0FFF;
        uint8_t nn = opcode & 0x00FF;
        uint8_t n = opcode & 0x000F;

        if (opcode != prev_opcode)
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
                        pc = stack[--sp];
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
                stack[sp++] = pc;
                pc = opcode & 0x0FFF;
                break;
            
            // Skip following instruction if VX == NN
            case 0x3000:
                if (v[x] == nn)
                    pc += 4;
                else
                    pc += 2;
                break;

            // Skip following instruction if VX != NN
            case 0x4000:
                if (v[x] != nn)
                    pc += 4;
                else
                    pc += 2;
                break;
            
            // Skip following instruction if VX == VY
            case 0x5000:
                if (v[x] == v[y])
                    pc += 4;
                else
                    pc += 2;
                break;

            // Store NN in VX
            case 0x6000:
                v[x] = nn;
                pc += 2;
                break;
            
            // Add NN to VX
            case 0x7000:
                v[x] += nn;
                pc += 2;
                break;

            case 0x8000:
                switch (opcode & 0x000F)
                {
                    // Store VY in VX
                    case 0x0:
                        v[x] = v[y];
                        pc += 2;
                        break;
                    
                    // Set VX to VX | VY
                    case 0x1:
                        v[x] |= v[y];
                        pc += 2;
                        break;

                    // Set VX to VX && VY
                    case 0x2:
                        v[x] &= v[y];
                        pc += 2;
                        break;

                    // Set VX to VX ^ VY
                    case 0x3:
                        v[x] ^= v[y];
                        pc += 2;
                        break;

                    // Add VY to VX, set VF if carry occurs
                    case 0x4:
                        if ((uint64_t) (v[opcode & 0x0F00]) + (uint64_t) (v[opcode & 0x00F0]) > UINT8_MAX)
                            v[0xF] = 1;
                        else
                            v[0xF] = 0;
                        v[x] += v[y];
                        pc += 2;
                        break;

                    case 0x5:
                        //Subtract value of VY from VX
                        // Set VF to 0 if a borrow occurs, set to 1 otherwise
                        todo(opcode);
                        break;
                    
                    // Set VX to (VY >> 1), set VF to LSB prior to the shift
                    case 0x6:
                        v[x] = v[y] >> 1;
                        v[0xF] = v[y] & 0xF;
                        pc += 2;
                        break;
                    
                    case 0x7:
                        // Set VX to the value of VY minus VX
                        // Set VF to 0 if a borrow occurs, set to 1 otherwise
                        todo(opcode);
                        break;
                    
                    // Set VX to (VY << 1), set VF to MSB prior to the shift
                    case 0xE:
                        v[x] = v[y] << 1;
                        v[0xF] = v[x] & 0xF;
                        pc += 2;
                        break;

                    default:
                        break;
                }
                break;
            
            // Skip next instruction if VX != VY
            case 0x9000:
                if (v[x] != v[y])
                    pc += 4;
                else
                    pc += 2;
                break;
            
            // Store NNN in I
            case 0xA000:
                I = nnn;
                pc += 2;
                break;
            
            // Jump to NNN + V0
            case 0xB000:
                pc = v[0] + nnn;
                break;

            // Set VX to (random number & NN)
            case 0xC000:
                v[x] = rand() & nn;
                break;
            
            case 0xD000:
                // Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I. 
                // Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
                for (uint8_t y_sprite = 0; y < n; y++)
                {
                    for (uint8_t x_sprite = 0; x < 8; x++)
                    {
                        display[(v[y] + y_sprite) * WIDTH + v[x] + x_sprite] = 
                        display[v[y] * WIDTH + v[x]] ^ mem[I + y];
                    }
                }
                pc += 2;
                break;

            case 0xE000:
                switch (opcode & 0x00FF)
                {
                    // Skip the following instruction if the key corresponding
                    // to the hex value currently stored in register VX is pressed
                    case 0x9E:
                        //if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == )
                        todo(opcode);
                        break;

                    // Skip the following instruction if the key corresponding
                    // to the hex value currently stored in register VX is not pressed
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
                    // Store delay timer in VX
                    case 0x07:
                        v[x] = dt;
                        pc += 2;
                        break;
                    
                    // Wait for a keypress and store result in VX
                    case 0x0A:
                        todo(opcode);
                        break;

                    // Set delay timer to value in VX
                    case 0x15:
                        dt = v[x];
                        pc += 2;
                        break;

                    // Set sound timer to value in VX
                    case 0x18:
                        st = v[x];
                        pc += 2;
                        break;
                    
                    // Add value in VX to I
                    case 0x1E:
                        I += v[x];
                        pc += 2;
                        break;
                        
                    // Set I to the address of the sprite data corresponding
                    // to the hex digit stored in VX
                    case 0x29:
                        I = v[x] * 5;
                        pc += 2;
                        break;

                    // Store BCD of value in VX at I, I+1 and I+2
                    case 0x33:
                        mem[I] = v[x] / 100;
                        mem[I + 1] = (v[x] / 10) % 10;
                        mem[I + 2] = (v[x] % 100) % 10;
                        pc += 2;
                        break;

                    // Store values in V0 to VX (included) in memory at I to I + X
                    // I is set to I + X + 1
                    case 0x55:
                        for (uint8_t i = 0; i <= x; i++)
                            mem[I++] = v[i];
                        pc += 2;
                        break;

                    // Store values in memory at I to I + X in V0 to VX (included)
                    // I is set to I + X + 1
                    case 0x65:
                        for (uint8_t i = 0; i <= x; i++)
                            v[i] = mem[I++];
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

        // Sleep for 2000 microseconds (1 / 500 seconds)
        usleep(2000);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}