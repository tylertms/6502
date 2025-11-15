#pragma once
#include <stdint.h>
#include "cpu.h"

typedef struct _state {
    // CPU
    uint8_t ra, rx, ry;     // a/x/y registers
    uint8_t status;         // status register
    uint8_t stack;          // stack pointer
    uint16_t pc;            // program counter
    uint8_t cycles;         // cycle counter
    uint16_t addr;          // abs/rel addr
    uint8_t data;           // data byte
    uint8_t stop;           // execution stopped
    _instruction instr;     // current instruction

    // MEM
    uint8_t ram[0x0800];    // internal memory
    uint8_t rom[0x4000];    // cartridge memory (mapper 0)

    // PPU
    uint8_t ppu_reg[0x08];   // 8 PPU registers

} _state;
