cmake_minimum_required(VERSION 3.15)
project(6502 LANGUAGES C)

add_executable(6502
    src/bus.c
    src/cpu.c
    src/main.c
    src/mem.c
    src/ppu.c
)
