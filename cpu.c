#include "cpu.h"
#include <stdint.h>
#include <stdio.h>

void write(_state* state, uint16_t addr, uint8_t data) {
    state->ram[addr & 0xFFFF] = data;
}

uint8_t read(_state* state, uint16_t addr) {
    return state->ram[addr & 0xFFFF];
}

uint8_t fetch(_state* state) {
    if (!is_imp(state)) {
        state->data = read(state, state->addr);
    }
    return state->data;
}

void set_flag(_state* state, _flag flag, uint8_t value) {
    if (value) state->status |= flag;
    else state->status &= ~flag;
}

uint8_t get_flag(_state* state, _flag flag) {
    return (state->status & flag) ? 1 : 0;
}

uint8_t is_imp(_state* state) {
    return state->instr.addr_mode == _am_imp;
}

void branch(_state* state) {
    state->cycles++;
    state->addr += state->pc;

    if ((state->addr & 0xFF00) != (state->pc & 0xFF00)) {
        state->cycles++;
    }

    state->pc = state->addr;
}

void reset(_state* state) {
    uint16_t low = read(state, RST_VECTOR);
    uint16_t high = read(state, RST_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->ra = 0x00;
    state->rx = 0x00;
    state->ry = 0x00;

    state->stack = 0xFD;
    state->status = 0x00 | U;

    state->addr = 0x0000;
    state->data = 0x00;

    state->cycles = 8;
    state->stop = 0;
}

void irq(_state* state) {
    if (!get_flag(state, I)) {
        return;
    }

    write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 0);
    set_flag(state, U, 1);
    set_flag(state, I, 1);
    write(state, 0x0100 + state->stack--, state->status);

    uint16_t low = read(state, IRQ_VECTOR);
    uint16_t high = read(state, IRQ_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 7;
}

void nmi(_state* state) {
    write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 0);
    set_flag(state, U, 1);
    set_flag(state, I, 1);
    write(state, 0x0100 + state->stack--, state->status);

    uint16_t low = read(state, NMI_VECTOR);
    uint16_t high = read(state, NMI_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 8;
}

void clock(_state* state) {
    if (state->cycles--) {
        return;
    }

    uint8_t opcode = read(state, state->pc++);
    if (!opcode) state->stop = 1;

    state->instr = instructions[opcode];
    state->cycles = state->instr.cycle_count;

    uint8_t am_cycle = state->instr.ex_am(state);
    uint8_t op_cycle = state->instr.ex_op(state);

    print_state(state);

    state->cycles += (am_cycle & op_cycle);
}

void print_state(_state* state) {
    printf("OP: %s, AM: %s\n", state->instr.op_name, state->instr.am_name);
    printf("A: 0x%x, X: 0x%x, Y: 0x%x, STK: 0x%x, STAT: 0x%x\n",
        state->ra, state->rx, state->ry, state->stack, state->status);

    printf("$%.4x:", 0);
    for (int x = 0; x < 16; x++) {
        printf(" %.2x", state->ram[x]);
    }
    printf("\n");

    printf("$%.4x:", 0x8000);
    for (int x = 0; x < 16; x++) {
        printf(" %.2x", state->ram[0x8000 + x]);
    }
    printf("\n");
}

int main() {
    uint8_t ram[0x10000];
    _state state = {};
    state.ram = ram;

    const uint16_t prog_start = 0x8000;
    const uint16_t prog_len = 28;
    const uint8_t prog[] = {0xA2, 0x0A, 0x8E, 0x00, 0x00, 0xA2, 0x03, 0x8E, 0x01, 0x00, 0xAC, 0x00, 0x00, 0xA9, 0x00, 0x18, 0x6D, 0x01, 0x00, 0x88, 0xD0, 0xFA, 0x8D, 0x02, 0x00, 0xEA, 0xEA, 0xEA};

    for (uint16_t offset = 0; offset < prog_len; offset++) {
        state.ram[prog_start + offset] = prog[offset];
    }

    ram[RST_VECTOR] = 0x00;
    ram[RST_VECTOR + 1] = 0x80;

    reset(&state);

    while (!state.stop) {
        clock(&state);
    }

    return 0;
}
