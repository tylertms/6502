#include "ppu.h"

uint8_t ppu_read(_state* state, uint16_t addr) {
    return state->ppu_reg[addr & 0x07];
}

void ppu_write(_state* state, uint16_t addr, uint8_t data) {
    state->ppu_reg[addr & 0x07] = data;
}
