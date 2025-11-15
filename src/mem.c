#include "mem.h"
#include "main.h"

void ram_write(_state* state, uint16_t addr, uint8_t data) {
    state->ram[addr & 0x07FF] = data;
}

uint8_t ram_read(_state* state, uint16_t addr) {
    return state->ram[addr & 0x07FF];
}

uint8_t rom_read(_state* state, uint16_t addr) {
    return state->rom[addr & 0x3FFF];
}
