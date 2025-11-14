#include "cpu.h"
#include "mem.h"
#include <stdio.h>

void cpu_clock(_state* state) {
    if (state->cycles--) {
        return;
    }

    uint8_t opcode = mem_read(state, state->pc++);

    state->instr = instructions[opcode];
    state->cycles = state->instr.cycle_count;

    uint8_t am_cycle = state->instr.ex_am(state);
    uint8_t op_cycle = state->instr.ex_op(state);

    state->cycles += (am_cycle & op_cycle);

    print_state(state);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cpu_reset(_state* state) {
    uint16_t low = mem_read(state, RST_VECTOR);
    uint16_t high = mem_read(state, RST_VECTOR + 1);
    state->pc = (high << 8) | low;

    printf("STARTING EXECUTION AT: 0x%x\n", state->pc);

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

void cpu_irq(_state* state) {
    if (!get_flag(state, I)) {
        return;
    }

    mem_write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    mem_write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 0);
    set_flag(state, U, 1);
    set_flag(state, I, 1);
    mem_write(state, 0x0100 + state->stack--, state->status);

    uint16_t low = mem_read(state, IRQ_VECTOR);
    uint16_t high = mem_read(state, IRQ_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 7;
}

void cpu_nmi(_state* state) {
    mem_write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    mem_write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 0);
    set_flag(state, U, 1);
    set_flag(state, I, 1);
    mem_write(state, 0x0100 + state->stack--, state->status);

    uint16_t low = mem_read(state, NMI_VECTOR);
    uint16_t high = mem_read(state, NMI_VECTOR + 1);
    state->pc = (high << 8) | low;

    state->cycles = 8;
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
    uint16_t res = state->addr + state->pc;

    if ((res & 0xFF00) != (state->pc & 0xFF00)) {
        state->cycles++;
    }

    state->pc = res;
}

void print_state(_state* state) {
    for (int x = 0; x < 102; x++) {
        printf("-");
    }

    printf("\nOP: %s | AM: %s | PC: 0x%x | A: 0x%x | X: 0x%x | Y: 0x%x | STK: 0x%x | STAT: 0x%x\n",
        state->instr.op_name, state->instr.am_name, state->pc,
        state->ra, state->rx, state->ry, state->stack, state->status);

    for (int y = 0; y < 2; y++) {
        printf("$%.4x:", y << 5);
        for (int x = 0; x < (1 << 5); x++) {
            printf(" %.2x", state->ram[(y << 5) + x]);
        }
        printf("\n");
    }

    printf("Press Enter to continue...\n\n");
}

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
    state->addr = mem_read(state, state->pc++);
    return 0;
}

uint8_t am_zpx(_state* state) {
    state->addr = (mem_read(state, state->pc++) + state->rx) & 0xFF;
    return 0;
}

uint8_t am_zpy(_state* state) {
    state->addr = (mem_read(state, state->pc++) + state->ry) & 0xFF;
    return 0;
}

uint8_t am_abs(_state* state) {
    uint16_t low = mem_read(state, state->pc++);
    uint16_t high = mem_read(state, state->pc++);
    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_abx(_state* state) {
    uint16_t low = mem_read(state, state->pc++);
    uint16_t high = mem_read(state, state->pc++);
    state->addr = ((high << 8) | low) + state->rx;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_aby(_state* state) {
    uint16_t low = mem_read(state, state->pc++);
    uint16_t high = mem_read(state, state->pc++);
    state->addr = ((high << 8) | low) + state->ry;
    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_idr(_state* state) {
    uint16_t p_low = mem_read(state, state->pc++);
    uint16_t p_high = mem_read(state, state->pc++);
    uint16_t ptr = (p_high << 8) | p_low;

    uint16_t high_addr = (p_low == 0xFF) ? (ptr & 0xFF00) : (ptr + 1);
    state->addr = (mem_read(state, high_addr) << 8) | mem_read(state, ptr);

    return 0;
}

uint8_t am_idx(_state* state) {
    uint16_t base = mem_read(state, state->pc++);
    uint16_t low = mem_read(state, (base + state->rx) & 0x00FF);
    uint16_t high = mem_read(state, (base + state->rx + 1) & 0x00FF);

    state->addr = (high << 8) | low;
    return 0;
}

uint8_t am_idy(_state* state) {
    uint16_t base = mem_read(state, state->pc++);

    uint16_t low = mem_read(state, base & 0x00FF);
    uint16_t high = mem_read(state, (base + 1) & 0x00FF);
    state->addr = ((high << 8) | low) + state->ry;

    return (state->addr & 0xFF00) != (high << 8);
}

uint8_t am_rel(_state* state) {
    state->addr = mem_read(state, state->pc++);
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
    uint8_t memory = mem_fetch(state);
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
    uint8_t memory = mem_fetch(state);
    state->ra &= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_asl(_state* state) {
    uint8_t memory = mem_fetch(state);
    uint16_t res = (uint16_t)memory << 1;

    set_flag(state, C, res > 255);
    set_flag(state, Z, (res & 0xFF) == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res & 0xFF;
    } else {
        mem_write(state, state->addr, res & 0xFF);
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
    uint8_t memory = mem_fetch(state);
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

    mem_write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    mem_write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    set_flag(state, B, 1);
    mem_write(state, 0x0100 + state->stack--, state->status);
    set_flag(state, B, 0);

    uint16_t low = mem_read(state, IRQ_VECTOR);
    uint16_t high = mem_read(state, IRQ_VECTOR + 1);
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
    uint8_t memory = mem_fetch(state);

    set_flag(state, C, state->ra >= memory);
    set_flag(state, Z, state->ra == memory);
    set_flag(state, N, state->ra < memory);

    return 1;
}

uint8_t op_cpx(_state* state) {
    uint8_t memory = mem_fetch(state);

    set_flag(state, C, state->rx >= memory);
    set_flag(state, Z, state->rx == memory);
    set_flag(state, N, state->rx < memory);

	return 0;
}

uint8_t op_cpy(_state* state) {
    uint8_t memory = mem_fetch(state);

    set_flag(state, C, state->ry >= memory);
    set_flag(state, Z, state->ry == memory);
    set_flag(state, N, state->ry < memory);

	return 0;
}

uint8_t op_dec(_state* state) {
    uint8_t memory = mem_fetch(state);
    memory--;
    mem_write(state, state->addr, memory);

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
    uint8_t memory = mem_fetch(state);
    state->ra ^= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_inc(_state* state) {
    uint8_t memory = mem_fetch(state);
    memory++;
    mem_write(state, state->addr, memory);

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

    mem_write(state, 0x0100 + state->stack--, (state->pc >> 8) & 0xFF);
    mem_write(state, 0x0100 + state->stack--, state->pc & 0xFF);

    state->pc = state->addr;

	return 0;
}

uint8_t op_lda(_state* state) {
    uint8_t memory = mem_fetch(state);
    state->ra = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_ldx(_state* state) {
    uint8_t memory = mem_fetch(state);
    state->rx = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_ldy(_state* state) {
    uint8_t memory = mem_fetch(state);
    state->ry = memory;

    set_flag(state, Z, memory == 0x00);
    set_flag(state, N, memory & 0x80);

	return 1;
}

uint8_t op_lsr(_state* state) {
    uint8_t memory = mem_fetch(state);
    uint8_t res = memory >> 1;

    set_flag(state, C, memory & 0x01);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, 0);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        mem_write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_nop(_state* state) {
	return 0;
}

uint8_t op_ora(_state* state) {
    uint8_t memory = mem_fetch(state);
    state->ra |= memory;

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 1;
}

uint8_t op_pha(_state* state) {
    mem_write(state, 0x0100 + state->stack--, state->ra);

	return 0;
}

uint8_t op_php(_state* state) {
    mem_write(state, 0x0100 + state->stack--, state->status | B);

	return 0;
}

uint8_t op_pla(_state* state) {
    state->ra = mem_read(state, 0x0100 + ++state->stack);

    set_flag(state, Z, state->ra == 0x00);
    set_flag(state, N, state->ra & 0x80);

	return 0;
}

uint8_t op_plp(_state* state) {
    state->status = mem_read(state, 0x0100 + ++state->stack);

    return 0;
}

uint8_t op_rol(_state* state) {
    uint8_t memory = mem_fetch(state);
    uint8_t res = (memory << 1) | get_flag(state, C);

    set_flag(state, C, memory & 0x80);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        mem_write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_ror(_state* state) {
    uint8_t memory = mem_fetch(state);
    uint8_t res = (memory >> 1) | (get_flag(state, C) << 7);

    set_flag(state, C, memory & 0x01);
    set_flag(state, Z, res == 0x00);
    set_flag(state, N, res & 0x80);

    if (is_imp(state)) {
        state->ra = res;
    } else {
        mem_write(state, state->addr, res);
    }

	return 0;
}

uint8_t op_rti(_state* state) {
    state->status = mem_read(state, 0x0100 + ++state->stack);
    state->pc = mem_read(state, 0x0100 + ++state->stack);
    state->pc |= (uint16_t)mem_read(state, 0x0100 + ++state->stack) << 8;

	return 0;
}

uint8_t op_rts(_state* state) {
    state->pc = mem_read(state, 0x0100 + ++state->stack) + 1;
    state->pc |= (uint16_t)mem_read(state, 0x0100 + ++state->stack) << 8;

	return 0;
}

uint8_t op_sbc(_state* state) {
    uint8_t memory = mem_fetch(state);
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
    mem_write(state, state->addr, state->ra);
	return 0;
}

uint8_t op_stx(_state* state) {
    mem_write(state, state->addr, state->rx);
	return 0;
}

uint8_t op_sty(_state* state) {
    mem_write(state, state->addr, state->ry);
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
    //state->stop = 1;
    return 0;
}
