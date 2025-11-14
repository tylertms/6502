cmake_minimum_required(VERSION 3.15)
project(6502 LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SDL3)

add_executable(6502 cpu.c)
target_link_libraries(6502 PRIVATE SDL3::SDL3)
