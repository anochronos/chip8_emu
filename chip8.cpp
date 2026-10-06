#include "chip8.hpp"
#include <algorithm>
#include <stdexcept>



template <typename T, std::size_t N>
void clearVals(T (&arr)[N]) {
    std::fill(std::begin(arr), std::end(arr), T{});
}


void chip8::initialize() {
    pc = START_ADD;         // counter starts at 0x200
    opcode = 0;             // reset current opcode
    I = 0;                  // reset index register 
    sp = 0;                 // reset stack pointer
    
    clearVals(gfx);        // clear display by setting all pixels to 0
    clearVals(stack);      // clear stack
    clearVals(V);          // clear registers
    clearVals(memory);     // clear memory

    // load fontset
    for(int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

void chip8::loadGame(chip8& chip, const char* path) {
    // opening file
    FILE* f = fopen(path, "rb");                                
    if (f == nullptr) {
        throw std::runtime_error("could not open ROM file");
    }

    // reading file
    size_t n = fread(&memory[0x200], 1, MEM_SIZE - START_ADD, f);
    if (n == 0) {
        throw std::runtime_error("ROM file was empty or could not be read");
    }

    // close file
    fclose(f);

}

void chip8::emulateCycle() {
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcode
    switch(opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F)
            {
                case 0x0000:
                    clearVals(gfx);
                    pc += 2;
                break;

                case 0x000E:
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                break;

                default:
                    printf ("Unknown opcode [0x0000] : 0x%X\n", opcode);
            }
        break;

        case 0x1000:
            pc = opcode & 0x0FFF;
        break;

        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
        break;

        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                pc += 4;
            }
            else {
                pc += 2;
            }
        break;

        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                pc += 4;
            }
            else {
                pc += 2;
            }
        break;

        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
        break;

        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
        break;

        case 0x8000:
            switch(opcode & 0x000F) 
            {
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0004:
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
                        V[0xF] = 1; // carry
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;          
                break;

                case 0x0005:
                    if(V[(opcode & 0x00F0) >> 4] <= V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;          
                break;

                case 0x0006: {
                    unsigned char bit = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    V[0xF] = bit;
                    pc += 2;
                break;
                }

                case 0x0007:
                    if(V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8]) {
                            V[0xF] = 1;
                        } else {
                            V[0xF] = 0;
                        }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x000E:
                break;

                    
            }
        
        default:
        break;

    }
}

