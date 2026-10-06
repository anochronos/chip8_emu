#include <SDL2/SDL.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "chip8.hpp"

static const int SCREEN_W = 64;
static const int SCREEN_H = 32;
static const int SCALE = 10;             // window is 640 x 320
static const int CYCLES_PER_FRAME = 10;  // 10 cycles * 60 frames = about 600 instructions per second
static const Uint32 FRAME_MS = 16;       // about 60 frames per second

// Index is the CHIP-8 key (0 to F), value is the physical key that triggers it
static const SDL_Scancode keymap[16] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom file>\n";
        return 1;
    }

    chip8 myChip8;

    // loadGame calls initialize() itself
    try {
        myChip8.loadGame(myChip8, argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W * SCALE, SCREEN_H * SCALE,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // One texel per CHIP-8 pixel, SDL scales it up to the window size
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_W, SCREEN_H);
    if (texture == nullptr) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    uint32_t pixels[SCREEN_W * SCREEN_H];
    bool running = true;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        // Window close and Esc
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Fill key[] with 1 or 0 for every key, so releases get recorded too
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < 16; ++i) {
            myChip8.key[i] = state[keymap[i]] ? 1 : 0;
        }

        // Run the CPU, then tick the timers once (60 Hz)
        for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
            myChip8.emulateCycle();
        }
        myChip8.updateTimers();

        // Rebuild the texture only when the screen changed
        if (myChip8.drawFlag) {
            for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) {
                pixels[i] = myChip8.gfx[i] ? 0xFFFFFFFF : 0xFF000000;
            }
            SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_W * sizeof(uint32_t));
            myChip8.drawFlag = false;
        }

        // Present every frame so the window repaints correctly after being covered
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // Hold the loop to about 60 frames per second
        Uint32 elapsed = SDL_GetTicks() - frameStart;
        if (elapsed < FRAME_MS) {
            SDL_Delay(FRAME_MS - elapsed);
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}