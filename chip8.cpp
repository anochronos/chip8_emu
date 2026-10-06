#include "chip8.hpp"
#include <algorithm>
#include <stdexcept>

template <typename T, std::size_t N>
void clearVals(T (&arr)[N]) {
    std::fill(std::begin(arr), std::end(arr), T{});
}


void chip8::initialize() {
    pc = 0x200;             // counter starts at 0x200
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
    FILE* f = fopen(path, "rb");
    if (f == nullptr) {
        throw std::runtime_error("could not open ROM file");
    }
}

