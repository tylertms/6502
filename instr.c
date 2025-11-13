#include "cpu.h"

/* address modes */

uint8_t am_acc(_state* state) {
    state->data = state->ra;
    return 0;
}

uint8_t am_imp(_state* state) {
    state->data = state->ra;
    return 0;
}

uint8_t am_imm(_state* state) {
    state->addr = state->pc++;
    return 0;
}

uint8_t am_zpg(_state* state) {
    state->addr = read(state->pc++);
    return 0;
}

uint8_t am_zpx(_state* state) {
    state->addr = (read(state->pc++) + state->rx) & 0xFF;
    return 0;
}

uint8_t am_zpy(_state* state) {
    state->addr = (read(state->pc++) + state->ry) & 0xFF;
    return 0;
}

uint8_t am_abs(_state* state) {
    uint16_t low = read(state->pc++);
    uint16_t high = read(state->pc++);
    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_abx(_state* state) {
    uint16_t low = read(state->pc++);
    uint16_t high = read(state->pc++);
    state->addr = ((high << 8) | low) + state->rx;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_aby(_state* state) {
    uint16_t low = read(state->pc++);
    uint16_t high = read(state->pc++);
    state->addr = ((high << 8) | low) + state->ry;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_idr(_state* state) {
    uint16_t p_low = read(state->pc++);
    uint16_t p_high = read(state->pc++);
    uint16_t ptr = (p_high << 8) | p_low;

    uint16_t high_addr = (p_low == 0xFF) ? (ptr & 0xFF00) : (ptr + 1);
    state->addr = (read(high_addr) << 8) | read(ptr);

    return 0;
}

uint8_t am_idx(_state* state) {
    uint16_t base = read(state->pc++);
    uint16_t low = read((base + state->rx) & 0x00FF);
    uint16_t high = read((base + state->rx + 1) & 0x00FF);

    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_idy(_state* state) {
    uint16_t base = read(state->pc++);

    uint16_t low = read(base & 0x00FF);
    uint16_t high = read((base + 1) & 0x00FF);
    state->addr = ((high << 8) | low) + state->ry;

    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_rel(_state* state) {
    state->addr = read(state->pc++);
    if (state->addr & 0x80) {
        state->addr |= 0xFF00;
    }

    return 0;
}

uint8_t am____(_state* state) {
    op____(state);
    return 0;
}
