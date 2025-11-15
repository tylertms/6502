#include "bus.h"
#include "mem.h"
#include "ppu.h"
#include <stdio.h>

void bus_write(_state* state, uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <=0x1FFF) {
        ram_write(state, addr, data);
    } else if (0x2000 <= addr && addr <=0x3FFF) {
        ppu_write(state, addr, data);
    } else {
        fprintf(stderr, "ERROR: Invalid memory address write: 0x%x to 0x%x\n", data, addr);
        state->stop = 1;
    }
}

uint8_t bus_read(_state* state, uint16_t addr) {
    if (0x0000 <= addr && addr <=0x1FFF) {
        return ram_read(state, addr);
    } else if (0x2000 <= addr && addr <=0x3FFF) {
        return ppu_read(state, addr);
    } else if (0x8000 <= addr && addr <= 0xFFFF) {
        return rom_read(state, addr);
    } else {
        fprintf(stderr, "ERROR: Invalid memory address read: 0x%x\n", addr);
        state->stop = 1;
        return 0xFF;
    }
}

uint8_t bus_fetch(_state* state) {
    if (!is_imp(state)) {
        state->data = bus_read(state, state->addr);
    }
    return state->data;
}
