#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

    // Fonts
    // ...

    for (;;)
    {

        switch (opcode)
        {
            
        }
    }

    return EXIT_SUCCESS;
}