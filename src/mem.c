#include "mem.h"
#include <stdio.h>

void mem_write(_state* state, uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <=0x1FFF) {
        state->ram[addr & 0x07FF] = data;
        return;
    }

    fprintf(stderr, "ERROR: Invalid memory address write: 0x%x to 0x%x\n", data, addr);
    state->stop = 1;
}

uint8_t mem_read(_state* state, uint16_t addr) {
    if (0x0000 <= addr && addr <=0x1FFF) {
        return state->ram[addr & 0x07FF];
    }

    if (0x8000 <= addr && addr <= 0xFFFF) {
        return state->rom[addr & 0x3FFF];
    }

    fprintf(stderr, "ERROR: Invalid memory address read: 0x%x\n", addr);
    state->stop = 1;

    return 0xFF;
}

uint8_t mem_fetch(_state* state) {
    if (!is_imp(state)) {
        state->data = mem_read(state, state->addr);
    }
    return state->data;
}
