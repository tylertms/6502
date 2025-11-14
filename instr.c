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
    state->addr = read(state, state->pc++);
    return 0;
}

uint8_t am_zpx(_state* state) {
    state->addr = (read(state, state->pc++) + state->rx) & 0xFF;
    return 0;
}

uint8_t am_zpy(_state* state) {
    state->addr = (read(state, state->pc++) + state->ry) & 0xFF;
    return 0;
}

uint8_t am_abs(_state* state) {
    uint16_t low = read(state, state->pc++);
    uint16_t high = read(state, state->pc++);
    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_abx(_state* state) {
    uint16_t low = read(state, state->pc++);
    uint16_t high = read(state, state->pc++);
    state->addr = ((high << 8) | low) + state->rx;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_aby(_state* state) {
    uint16_t low = read(state, state->pc++);
    uint16_t high = read(state, state->pc++);
    state->addr = ((high << 8) | low) + state->ry;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_idr(_state* state) {
    uint16_t p_low = read(state, state->pc++);
    uint16_t p_high = read(state, state->pc++);
    uint16_t ptr = (p_high << 8) | p_low;

    uint16_t high_addr = (p_low == 0xFF) ? (ptr & 0xFF00) : (ptr + 1);
    state->addr = (read(state, high_addr) << 8) | read(state, ptr);

    return 0;
}

uint8_t am_idx(_state* state) {
    uint16_t base = read(state, state->pc++);
    uint16_t low = read(state, (base + state->rx) & 0x00FF);
    uint16_t high = read(state, (base + state->rx + 1) & 0x00FF);

    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_idy(_state* state) {
    uint16_t base = read(state, state->pc++);

    uint16_t low = read(state, base & 0x00FF);
    uint16_t high = read(state, (base + 1) & 0x00FF);
    state->addr = ((high << 8) | low) + state->ry;

    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_rel(_state* state) {
    state->addr = read(state, state->pc++);
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
    uint8_t memory = fetch(state);
    uint16_t res = (uint16_t)state->ra + (uint16_t)memory + get_flag(state, C);

    uint16_t overflow = (res ^ state->ra) & (res ^ memory) & 0x80;
    set_flag(state, C, res > 0xFF);
    set_flag(state, Z, (res & 0xFF) == 0x00);
    set_flag(state, V, overflow);
    set_flag(state, N, res & 0x80);

    state->ra = res & 0xFF;

	return 1;
}

uint8_t op_and(_state* state) {
    uint8_t memory = fetch(state);
    state->ra &= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_asl(_state* state) {
    uint8_t memory = fetch(state);
    uint16_t res = (uint16_t)memory << 1;

    set_flag(state, C, res > 255);
    set_flag(state, Z, (res & 0xFF) == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res & 0xFF;
    } else {
        write(state, state->addr, res & 0xFF);
    }

	return 0;
}

uint8_t op_bcc(_state* state) {
    if (!get_flag(state, C)) {
        branch(state);
    }

	return 0;
}

uint8_t op_bcs(_state* state) {
    if (get_flag(state, C)) {
        branch(state);
    }

	return 0;
}

uint8_t op_beq(_state* state) {
    if (get_flag(state, Z)) {
        branch(state);
    }

    return 0;
}

uint8_t op_bit(_state* state) {
    uint8_t memory = fetch(state);
    uint8_t res = state->ra & memory;

    set_flag(state, Z, res == 0x00);
    set_flag(state, V, memory & 0x40);
    set_flag(state, N, memory & 0x80);

	return 0;
}

uint8_t op_bmi(_state* state) {
    if (get_flag(state, N)) {
        branch(state);
    }

	return 0;
}

uint8_t op_bne(_state* state) {
    if (!get_flag(state, Z)) {
        branch(state);
    }

	return 0;
}

uint8_t op_bpl(_state* state) {
    if (!get_flag(state, N)) {
        branch(state);
    }

	return 0;
}

uint8_t op_brk(_state* state) {
    state->pc++;

    set_flag(state, I, 1);

    write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 1);
    write(state, 0x0100 + state->stack--, state->status);
    set_flag(state, B, 0);

    uint16_t low = read(state, IRQ_VECTOR);
    uint16_t high = read(state, IRQ_VECTOR + 1);
    state->pc = (high << 8) | low;

	return 0;
}

uint8_t op_bvc(_state* state) {
    if (!get_flag(state, V)) {
        branch(state);
    }

	return 0;
}

uint8_t op_bvs(_state* state) {
    if (get_flag(state, V)) {
        branch(state);
    }

	return 0;
}

uint8_t op_clc(_state* state) {
    set_flag(state, C, 0);
	return 0;
}

uint8_t op_cld(_state* state) {
    set_flag(state, D, 0);
	return 0;
}

uint8_t op_cli(_state* state) {
    set_flag(state, I, 0);
	return 0;
}

uint8_t op_clv(_state* state) {
    set_flag(state, V, 0);
	return 0;
}

uint8_t op_cmp(_state* state) {
    uint8_t memory = fetch(state);

    set_flag(state, C, state->ra >= memory);
    set_flag(state, Z, state->ra == memory);
    set_flag(state, N, state->ra < memory);

    return 1;
}

uint8_t op_cpx(_state* state) {
    uint8_t memory = fetch(state);

    set_flag(state, C, state->rx >= memory);
    set_flag(state, Z, state->rx == memory);
    set_flag(state, N, state->rx < memory);

	return 0;
}

uint8_t op_cpy(_state* state) {
    uint8_t memory = fetch(state);

    set_flag(state, C, state->ry >= memory);
    set_flag(state, Z, state->ry == memory);
    set_flag(state, N, state->ry < memory);

	return 0;
}

uint8_t op_dec(_state* state) {
    uint8_t memory = fetch(state);
    memory--;
    write(state, state->addr, memory);

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 0;
}

uint8_t op_dex(_state* state) {
    state->rx--;
    set_flag(state, Z, state->rx == 0x00);
    set_flag(state, N, state->rx & 0x80);

	return 0;
}

uint8_t op_dey(_state* state) {
    state->ry--;
    set_flag(state, Z, state->ry == 0x00);
    set_flag(state, N, state->ry & 0x80);

	return 0;
}

uint8_t op_eor(_state* state) {
    uint8_t memory = fetch(state);
    state->ra ^= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_inc(_state* state) {
    uint8_t memory = fetch(state);
    memory++;
    write(state, state->addr, memory);

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 0;
}

uint8_t op_inx(_state* state) {
    state->rx++;
    set_flag(state, Z, state->rx == 0x00);
    set_flag(state, N, state->rx & 0x80);

	return 0;
}

uint8_t op_iny(_state* state) {
    state->ry++;
    set_flag(state, Z, state->ry == 0x00);
    set_flag(state, N, state->ry & 0x80);

	return 0;
}

uint8_t op_jmp(_state* state) {
    state->pc = state->addr;

	return 0;
}

uint8_t op_jsr(_state* state) {
    state->pc--;

    write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    state->pc = state->addr;

	return 0;
}

uint8_t op_lda(_state* state) {
    uint8_t memory = fetch(state);
    state->ra = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_ldx(_state* state) {
    uint8_t memory = fetch(state);
    state->rx = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_ldy(_state* state) {
    uint8_t memory = fetch(state);
    state->ry = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_lsr(_state* state) {
    uint8_t memory = fetch(state);
    uint8_t res = memory >> 1;

    set_flag(state, C, memory & 0x01);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, 0);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_nop(_state* state) {
	return 0;
}

uint8_t op_ora(_state* state) {
    uint8_t memory = fetch(state);
    state->ra |= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_pha(_state* state) {
    write(state, 0x0100 + state->stack--, state->ra);

	return 0;
}

uint8_t op_php(_state* state) {
    write(state, 0x0100 + state->stack--, state->status | B);

	return 0;
}

uint8_t op_pla(_state* state) {
    state->ra = read(state, 0x0100 + ++state->stack);

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 0;
}

uint8_t op_plp(_state* state) {
    state->status = read(state, 0x0100 + ++state->stack);

    return 0;
}

uint8_t op_rol(_state* state) {
    uint8_t memory = fetch(state);
    uint8_t res = (memory << 1) | get_flag(state, C);

    set_flag(state, C, memory & 0x80);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_ror(_state* state) {
    uint8_t memory = fetch(state);
    uint8_t res = (memory >> 1) | (get_flag(state, C) << 7);

    set_flag(state, C, memory & 0x01);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_rti(_state* state) {
    state->status = read(state, 0x0100 + ++state->stack);
    state->pc = read(state, 0x0100 + ++state->stack);
    state->pc |= (uint16_t)read(state, 0x0100 + ++state->stack) << 8;

	return 0;
}

uint8_t op_rts(_state* state) {
    state->pc = read(state, 0x0100 + ++state->stack) + 1;
    state->pc |= (uint16_t)read(state, 0x0100 + ++state->stack) << 8;

	return 0;
}

uint8_t op_sbc(_state* state) {
    uint8_t memory = fetch(state);
    uint16_t value = (uint16_t)memory ^ 0xFF;
    uint16_t res = (uint16_t)state->ra + value + get_flag(state, C);

    uint16_t overflow = (res ^ state->ra) & (res ^ value) & 0x80;
    set_flag(state, C, res > 0xFF);
    set_flag(state, Z, (res & 0xFF) == 0x00);
    set_flag(state, V, overflow);
    set_flag(state, N, res & 0x80);

    state->ra = res & 0xFF;

    return 1;
}

uint8_t op_sec(_state* state) {
    set_flag(state, C, 1);
	return 0;
}

uint8_t op_sed(_state* state) {
    set_flag(state, D, 1);
	return 0;
}

uint8_t op_sei(_state* state) {
    set_flag(state, I, 1);
	return 0;
}

uint8_t op_sta(_state* state) {
    write(state, state->addr, state->ra);
	return 0;
}

uint8_t op_stx(_state* state) {
    write(state, state->addr, state->rx);
	return 0;
}

uint8_t op_sty(_state* state) {
    write(state, state->addr, state->ry);
	return 0;
}

uint8_t op_tax(_state* state) {
    state->rx = state->ra;

    set_flag(state, Z, state->rx == 0x00);
    set_flag(state, N, state->rx & 0x80);

	return 0;
}

uint8_t op_tay(_state* state) {
    state->ry = state->ra;

    set_flag(state, Z, state->ry == 0x00);
    set_flag(state, N, state->ry & 0x80);

	return 0;
}

uint8_t op_tsx(_state* state) {
    state->rx = state->stack;

    set_flag(state, Z, state->rx == 0x00);
    set_flag(state, N, state->rx & 0x80);

	return 0;
}

uint8_t op_txa(_state* state) {
    state->ra = state->rx;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 0;
}

uint8_t op_txs(_state* state) {
    state->stack = state->rx;

    set_flag(state, Z, state->stack == 0x00);
    set_flag(state, N, state->stack & 0x80);

	return 0;
}

uint8_t op_tya(_state* state) {
    state->ra = state->ry;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 0;
}

uint8_t op____(_state* state) {
    fprintf(stderr, "ERROR: Illegal instruction called!\nPC: 0x%x, Addr: 0x%x, Data: 0x%x, Op: 0x%x, AM: 0x%x\n",
        state->pc, state->addr, state->data, state->instr.operand, state->instr.addr_mode);
    state->stop = 1;
    return 0;
}
