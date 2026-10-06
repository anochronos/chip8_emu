cmake_minimum_required(VERSION 3.16)
project(chip8_emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

# Emulator core. It has no SDL dependency, so the tests can use it directly.
add_library(chip8_core src/chip8.cpp)
target_include_directories(chip8_core PUBLIC src)
target_compile_options(chip8_core PRIVATE -Wall -Wextra)

# Frontend: window, input, and the main loop
add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 PRIVATE chip8_core)
if(TARGET SDL2::SDL2main)
  # Must come before SDL2::SDL2 so the linker order is right on Windows
  target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(chip8 PRIVATE SDL2::SDL2)

# Tests
enable_testing()
add_executable(chip8_tests tests/test_chip8.cpp)
target_link_libraries(chip8_tests PRIVATE chip8_core)
add_test(NAME chip8_tests COMMAND chip8_tests)