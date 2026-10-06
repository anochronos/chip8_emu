#pragma once

class chip8{
    static constexpr int MEM_SIZE = 4096; 
    static constexpr int START_ADD = 0x200;

    // 35 opcodes of length 2 bytes
    unsigned short opcode;

    // total memory is 4k
    unsigned char memory[MEM_SIZE];

    // 16 registers 
    unsigned char V[16];

    // index register
    unsigned short I;

    // program counter
    unsigned short pc;

    

    // timer registers
    unsigned char delay_timer;
    unsigned char sound_timer;

    // stack
    unsigned short stack[16];

    // stack pointer
    unsigned short sp;

    
    
    unsigned char keymap[16] =
    {
        'x', '1', '2', '3',
        'q', 'w', 'e', 'a',
        's', 'd', 'z', 'c',
        '4', 'r', 'f', 'v'
    };

    // fontset
    unsigned char chip8_fontset[80] =
        { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

public:
    // store key state for the keypad
    unsigned char key[16];
        
    // screen size of 64x32 saving the pixel state
    // initializing it to 0 for all pixels off
    unsigned char gfx[64 * 32];
    bool drawFlag;

    chip8();
    ~chip8();
    void initialize();
    void updateTimers();
    void emulateCycle();
    void loadGame(chip8&, const char*);
};