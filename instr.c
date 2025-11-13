#include "cpu.h"
#include <stdio.h>

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
    return 0;
}

/* Operations */

uint8_t op_adc(_state* state) {
	return 0;
}

uint8_t op_and(_state* state) {
	return 0;
}

uint8_t op_asl(_state* state) {
	return 0;
}

uint8_t op_bcc(_state* state) {
	return 0;
}

uint8_t op_bcs(_state* state) {
	return 0;
}

uint8_t op_beq(_state* state) {
	return 0;
}

uint8_t op_bit(_state* state) {
	return 0;
}

uint8_t op_bmi(_state* state) {
	return 0;
}

uint8_t op_bne(_state* state) {
	return 0;
}

uint8_t op_bpl(_state* state) {
	return 0;
}

uint8_t op_brk(_state* state) {
	return 0;
}

uint8_t op_bvc(_state* state) {
	return 0;
}

uint8_t op_bvs(_state* state) {
	return 0;
}

uint8_t op_clc(_state* state) {
	return 0;
}

uint8_t op_cld(_state* state) {
	return 0;
}

uint8_t op_cli(_state* state) {
	return 0;
}

uint8_t op_clv(_state* state) {
	return 0;
}

uint8_t op_cmp(_state* state) {
	return 0;
}

uint8_t op_cpx(_state* state) {
	return 0;
}

uint8_t op_cpy(_state* state) {
	return 0;
}

uint8_t op_dec(_state* state) {
	return 0;
}

uint8_t op_dex(_state* state) {
	return 0;
}

uint8_t op_dey(_state* state) {
	return 0;
}

uint8_t op_eor(_state* state) {
	return 0;
}

uint8_t op_inc(_state* state) {
	return 0;
}

uint8_t op_inx(_state* state) {
	return 0;
}

uint8_t op_iny(_state* state) {
	return 0;
}

uint8_t op_jmp(_state* state) {
	return 0;
}

uint8_t op_jsr(_state* state) {
	return 0;
}

uint8_t op_lda(_state* state) {
	return 0;
}

uint8_t op_ldx(_state* state) {
	return 0;
}

uint8_t op_ldy(_state* state) {
	return 0;
}

uint8_t op_lsr(_state* state) {
	return 0;
}

uint8_t op_nop(_state* state) {
	return 0;
}

uint8_t op_ora(_state* state) {
	return 0;
}

uint8_t op_pha(_state* state) {
	return 0;
}

uint8_t op_php(_state* state) {
	return 0;
}

uint8_t op_pla(_state* state) {
	return 0;
}

uint8_t op_plp(_state* state) {
	return 0;
}

uint8_t op_rol(_state* state) {
	return 0;
}

uint8_t op_ror(_state* state) {
	return 0;
}

uint8_t op_rti(_state* state) {
	return 0;
}

uint8_t op_rts(_state* state) {
	return 0;
}

uint8_t op_sbc(_state* state) {
	return 0;
}

uint8_t op_sec(_state* state) {
	return 0;
}

uint8_t op_sed(_state* state) {
	return 0;
}

uint8_t op_sei(_state* state) {
	return 0;
}

uint8_t op_sta(_state* state) {
	return 0;
}

uint8_t op_stx(_state* state) {
	return 0;
}

uint8_t op_sty(_state* state) {
	return 0;
}

uint8_t op_tax(_state* state) {
	return 0;
}

uint8_t op_tay(_state* state) {
	return 0;
}

uint8_t op_tsx(_state* state) {
	return 0;
}

uint8_t op_txa(_state* state) {
	return 0;
}

uint8_t op_txs(_state* state) {
	return 0;
}

uint8_t op_tya(_state* state) {
	return 0;
}

uint8_t op____(_state* state) {
    fprintf(stderr, "ERROR: Illegal instruction called!\nPC: 0x%x, Addr: 0x%x, Data: 0x%x, Op: 0x%x, AM: 0x%x\n",
        state->pc, state->addr, state->data, state->instr.operand, state->instr.mode);
    state->stop = 1;
    return 0;
}
