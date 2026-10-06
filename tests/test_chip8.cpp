#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include "chip8.hpp"

static int failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "FAIL line " << __LINE__ << ": " #cond "\n";            \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

// Writes the bytes to a temporary ROM file, loads it, and runs some cycles.
// ROMs load at 0x200, so the first byte below sits at address 0x200.
static void runRom(chip8& c, std::initializer_list<unsigned char> bytes, int cycles) {
    const char* path = "test_rom.ch8";

    FILE* f = std::fopen(path, "wb");
    if (f == nullptr) {
        std::cerr << "could not create " << path << '\n';
        std::exit(1);
    }
    for (unsigned char b : bytes) {
        std::fputc(b, f);
    }
    std::fclose(f);

    c.loadGame(c, path);
    for (int i = 0; i < cycles; ++i) {
        c.emulateCycle();
    }
    std::remove(path);
}

static void testSetAndAdd() {
    chip8 c;
    // 6005: V0 = 5    7003: V0 += 3
    runRom(c, {0x60, 0x05, 0x70, 0x03}, 2);
    CHECK(c.getV(0) == 8);
}

static void testAddCarry() {
    chip8 c;
    // 60FF: V0 = 255   6102: V1 = 2   8014: V0 += V1
    runRom(c, {0x60, 0xFF, 0x61, 0x02, 0x80, 0x14}, 3);
    CHECK(c.getV(0) == 1);      // 257 wraps to 1
    CHECK(c.getV(0xF) == 1);    // carry flag set
}

static void testSubNoBorrow() {
    chip8 c;
    // 6005: V0 = 5   6103: V1 = 3   8015: V0 -= V1
    runRom(c, {0x60, 0x05, 0x61, 0x03, 0x80, 0x15}, 3);
    CHECK(c.getV(0) == 2);
    CHECK(c.getV(0xF) == 1);    // 1 means no borrow
}

static void testSubBorrow() {
    chip8 c;
    // 6003: V0 = 3   6105: V1 = 5   8015: V0 -= V1
    runRom(c, {0x60, 0x03, 0x61, 0x05, 0x80, 0x15}, 3);
    CHECK(c.getV(0) == 0xFE);   // wraps around
    CHECK(c.getV(0xF) == 0);    // 0 means a borrow happened
}

static void testSkipIfNotEqual() {
    chip8 c;
    // 6005: V0 = 5
    // 4006: skip next instruction if V0 != 6 (true, so skip)
    // 60AA: V0 = 0xAA (should be skipped)
    // 61BB: V1 = 0xBB
    runRom(c, {0x60, 0x05, 0x40, 0x06, 0x60, 0xAA, 0x61, 0xBB}, 3);
    CHECK(c.getV(0) == 5);
    CHECK(c.getV(1) == 0xBB);
}

static void testCallAndReturn() {
    chip8 c;
    // 0x200: 2206  call 0x206
    // 0x202: 61BB  V1 = 0xBB (runs after the return)
    // 0x204: 1204  jump to itself
    // 0x206: 60AA  V0 = 0xAA
    // 0x208: 00EE  return
    runRom(c, {0x22, 0x06, 0x61, 0xBB, 0x12, 0x04, 0x60, 0xAA, 0x00, 0xEE}, 4);
    CHECK(c.getV(0) == 0xAA);
    CHECK(c.getV(1) == 0xBB);
}

// Your turn: add tests for the shift opcodes (8XY6, 8XYE) and for FX33.

int main() {
    testSetAndAdd();
    testAddCarry();
    testSubNoBorrow();
    testSubBorrow();
    testSkipIfNotEqual();
    testCallAndReturn();

    if (failures == 0) {
        std::cout << "All tests passed\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}