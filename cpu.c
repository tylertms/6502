#include "cpu.h"
#include <stdint.h>

uint8_t ram[0x10000];

void write(uint16_t addr, uint8_t data) {
    ram[addr & 0xFFFF] = data;
}

uint8_t read(uint16_t addr) {
    return ram[addr & 0xFFFF];
}

uint8_t fetch(_state* state) {
    if (!state->)
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
    state->stop = 0;
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
    if (state->cycles--) {
        return;
    }

    uint8_t opcode = read(state->pc++);
    state->instr = instructions[opcode];
    state->cycles = state->instr.cycle_count;

    uint8_t am_cycle = state->instr.addr_mode(state);
    uint8_t op_cycle = state->instr.execute(state);

    state->cycles += (am_cycle & op_cycle);
}

int main() {
    _state state = {};

    const uint16_t prog_start = 0x8000;
    const uint16_t prog_len = 28;
    const uint8_t prog[] = {0xA2, 0x0A, 0x8E, 0x00, 0x00, 0xA2, 0x03, 0x8E, 0x01, 0x00, 0xAC, 0x00, 0x00, 0xA9, 0x00, 0x18, 0x6D, 0x01, 0x00, 0x88, 0xD0, 0xFA, 0x8D, 0x02, 0x00, 0xEA, 0xEA, 0xEA};

    for (uint16_t offset = 0; offset < prog_len; offset++) {
        ram[prog_start + offset] = prog[offset];
    }

    ram[RST_VECTOR] = 0x00;
    ram[RST_VECTOR + 1] = 0x80;

    reset(&state);

    while (!state.stop) {
        clock(&state);
    }

    return 0;
}
