cmake_minimum_required(VERSION 3.15)
project(6502 LANGUAGES C)

add_executable(6502 cpu.c)
target_compile_options(6502 PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
target_link_libraries(6502 PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
