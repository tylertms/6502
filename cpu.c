#include "cpu.h"
#include <stdint.h>
#include <stdio.h>

uint8_t ram[0x10000];

void write(uint16_t addr, uint8_t data) {
    ram[addr & 0xFFFF] = data;
}

uint8_t read(uint16_t addr) {
    return ram[addr & 0xFFFF];
}

void set_flag(_state* state, _flag flag) {
    state->status |= flag;
}

void rst_flag(_state* state, _flag flag) {
    state->status &= ~flag;
}

uint8_t get_flag(_state* state, _flag flag) {
    return state->status & flag;
}

void reset(_state* state) {
    uint16_t low = read(RST_VECTOR);
    uint16_t high = read(RST_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->ra = 0x00;
    state->rx = 0x00;
    state->ry = 0x00;

    state->stack = 0xFD;
    state->status = 0x00 | _U;

    state->addr = 0x0000;
    state->data = 0x00;

    state->cycles = 8;
}

void irq(_state* state) {
    if (!get_flag(state, _I)) {
        return;
    }

    write(0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(0x0100 + state->stack--, state->pc & 0xFF);

    rst_flag(state, _B);
    set_flag(state, _U);
    set_flag(state, _I);
    write(0x0100 + state->stack--, state->status);

    uint16_t low = read(IRQ_VECTOR);
    uint16_t high = read(IRQ_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 7;
}

void nmi(_state* state) {
    write(0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(0x0100 + state->stack--, state->pc & 0xFF);

    rst_flag(state, _B);
    set_flag(state, _U);
    set_flag(state, _I);
    write(0x0100 + state->stack--, state->status);

    uint16_t low = read(NMI_VECTOR);
    uint16_t high = read(NMI_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 8;
}

void clock(_state* state) {
    if (state->cycles) {
        state->cycles--;
        return;
    }
}

int main() {
    printf("%d, %d\n", instructions[0].execute(), instructions[0].cycle_count);
    return 0;
}
