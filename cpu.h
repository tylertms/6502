#pragma once
#include <stdint.h>

#define IN(operand, addr_mode, cycles) \
    {_op_##operand, _am_##addr_mode, op_##operand, am_##addr_mode, cycles}

#define ILLEGAL_INSTRCT \
    IN(___, ___, 0)

#define O(operand) op_##operand(_state* state)
#define A(mode) am_##mode(_state* state)

#define NMI_VECTOR 0xFFFA
#define RST_VECTOR 0xFFFC
#define IRQ_VECTOR 0xFFFE

typedef struct _state _state;

typedef enum _operand {
    _op_adc, _op_and, _op_asl, _op_bcc, _op_bcs, _op_beq, _op_bit, _op_bmi,
    _op_bne, _op_bpl, _op_brk, _op_bvc, _op_bvs, _op_clc, _op_cld, _op_cli,
    _op_clv, _op_cmp, _op_cpx, _op_cpy, _op_dec, _op_dex, _op_dey, _op_eor,
    _op_inc, _op_inx, _op_iny, _op_jmp, _op_jsr, _op_lda, _op_ldx, _op_ldy,
    _op_lsr, _op_nop, _op_ora, _op_pha, _op_php, _op_pla, _op_plp, _op_rol,
    _op_ror, _op_rti, _op_rts, _op_sbc, _op_sec, _op_sed, _op_sei, _op_sta,
    _op_stx, _op_sty, _op_tax, _op_tay, _op_tsx, _op_txa, _op_txs, _op_tya,
    _op____
} _operand;

typedef enum _mode {
    _am_acc, _am_imp, _am_imm,
    _am_zpg, _am_zpx, _am_zpy,
    _am_abs, _am_abx, _am_aby,
    _am_idr, _am_idx, _am_idy,
    _am_rel, _am____
} _mode;


uint8_t O(adc), O(and), O(asl), O(bcc), O(bcs), O(beq), O(bit), O(bmi),
        O(bne), O(bpl), O(brk), O(bvc), O(bvs), O(clc), O(cld), O(cli),
        O(clv), O(cmp), O(cpx), O(cpy), O(dec), O(dex), O(dey), O(eor),
        O(inc), O(inx), O(iny), O(jmp), O(jsr), O(lda), O(ldx), O(ldy),
        O(lsr), O(nop), O(ora), O(pha), O(php), O(pla), O(plp), O(rol),
        O(ror), O(rti), O(rts), O(sbc), O(sec), O(sed), O(sei), O(sta),
        O(stx), O(sty), O(tax), O(tay), O(tsx), O(txa), O(txs), O(tya),
        O(___);

uint8_t A(acc), A(imp), A(imm),
        A(zpg), A(zpx), A(zpy),
        A(abs), A(abx), A(aby),
        A(idr), A(idx), A(idy),
        A(rel), A(___);

typedef struct _instruction {
    _operand operand;
    _mode addr_mode;
    uint8_t (*ex_op)(_state*);
    uint8_t (*ex_am)(_state*);
    uint8_t cycle_count;
} _instruction;

typedef struct _state {
    uint8_t ra, rx, ry; // a/x/y registers
    uint8_t status;     // status register
    uint8_t stack;      // stack pointer
    uint16_t pc;        // program counter
    uint8_t cycles;     // cycle counter
    uint16_t addr;      // abs/rel addr
    uint8_t data;       // data byte
    uint8_t stop;       // execution stopped
    _instruction instr; // current instruction
} _state;

typedef enum _flag {
    C = (1 << 0),  // carry
    Z = (1 << 1),  // zero
    I = (1 << 2),  // interrupt disable
    D = (1 << 3),  // decimal mode (unused)
    B = (1 << 4),  // break/interrupt info
    U = (1 << 5),  // unused = 1
    V = (1 << 6),  // overflow
    N = (1 << 7),  // negative
} _flag;

static _instruction instructions[256] = {
    IN(brk, imp, 7), IN(ora, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(ora, zpg, 3), IN(asl, zpg, 5), ILLEGAL_INSTRCT,     // 0x00 - 0x07
    IN(php, imp, 3), IN(ora, imm, 2), IN(asl, acc, 2), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(ora, abs, 4), IN(asl, abs, 6), ILLEGAL_INSTRCT,     // 0x08 - 0x0F
    IN(bpl, rel, 2), IN(ora, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(ora, zpx, 4), IN(asl, zpx, 6), ILLEGAL_INSTRCT,     // 0x10 - 0x17
    IN(clc, imp, 2), IN(ora, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(ora, abx, 4), IN(asl, abx, 7), ILLEGAL_INSTRCT,     // 0x18 - 0x1F
    IN(jsr, abs, 6), IN(and, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(bit, zpg, 3), IN(and, zpg, 3), IN(rol, zpg, 5), ILLEGAL_INSTRCT,     // 0x20 - 0x27
    IN(plp, imp, 4), IN(and, imm, 2), IN(rol, acc, 2), ILLEGAL_INSTRCT, IN(bit, abs, 4), IN(and, abs, 4), IN(rol, abs, 6), ILLEGAL_INSTRCT,     // 0x28 - 0x2F
    IN(bmi, rel, 2), IN(and, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(and, zpx, 4), IN(rol, zpx, 6), ILLEGAL_INSTRCT,     // 0x30 - 0x37
    IN(sec, imp, 2), IN(and, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(and, abx, 4), IN(rol, abx, 7), ILLEGAL_INSTRCT,     // 0x38 - 0x3F
    IN(rti, imp, 6), IN(eor, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(eor, zpg, 3), IN(lsr, zpg, 5), ILLEGAL_INSTRCT,     // 0x40 - 0x47
    IN(pha, imp, 3), IN(eor, imm, 2), IN(lsr, acc, 2), ILLEGAL_INSTRCT, IN(jmp, abs, 3), IN(eor, abs, 4), IN(lsr, abs, 6), ILLEGAL_INSTRCT,     // 0x48 - 0x4F
    IN(bvc, rel, 2), IN(eor, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(eor, zpx, 4), IN(lsr, zpx, 6), ILLEGAL_INSTRCT,     // 0x50 - 0x57
    IN(cli, imp, 2), IN(eor, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(eor, abx, 4), IN(lsr, abx, 7), ILLEGAL_INSTRCT,     // 0x58 - 0x5F
    IN(rts, imp, 6), IN(adc, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(adc, zpg, 3), IN(ror, zpg, 5), ILLEGAL_INSTRCT,     // 0x60 - 0x67
    IN(pla, imp, 4), IN(adc, imm, 2), IN(ror, acc, 2), ILLEGAL_INSTRCT, IN(jmp, idr, 5), IN(adc, abs, 4), IN(ror, abs, 6), ILLEGAL_INSTRCT,     // 0x68 - 0x6F
    IN(bvs, rel, 2), IN(adc, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(adc, zpx, 4), IN(ror, zpx, 6), ILLEGAL_INSTRCT,     // 0x70 - 0x77
    IN(sei, imp, 2), IN(adc, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(adc, abx, 4), IN(ror, abx, 7), ILLEGAL_INSTRCT,     // 0x78 - 0x7F
    ILLEGAL_INSTRCT, IN(sta, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(sty, zpg, 3), IN(sta, zpg, 3), IN(stx, zpg, 3), ILLEGAL_INSTRCT,     // 0x80 - 0x87
    IN(dey, imp, 2), ILLEGAL_INSTRCT, IN(txa, imp, 2), ILLEGAL_INSTRCT, IN(sty, abs, 4), IN(sta, abs, 4), IN(stx, abs, 4), ILLEGAL_INSTRCT,     // 0x88 - 0x8F
    IN(bcc, rel, 2), IN(sta, idy, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(sty, zpx, 4), IN(sta, zpx, 4), IN(stx, zpy, 4), ILLEGAL_INSTRCT,     // 0x90 - 0x97
    IN(tya, imp, 2), IN(sta, aby, 5), IN(txs, imp, 2), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(sta, abx, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT,     // 0x98 - 0x9F
    IN(ldy, imm, 2), IN(lda, idx, 6), IN(ldx, imm, 2), ILLEGAL_INSTRCT, IN(ldy, zpg, 3), IN(lda, zpg, 3), IN(ldx, zpg, 3), ILLEGAL_INSTRCT,     // 0xA0 - 0xA7
    IN(tay, imp, 2), IN(lda, imm, 2), IN(tax, imp, 2), ILLEGAL_INSTRCT, IN(ldy, abs, 4), IN(lda, abs, 4), IN(ldx, abs, 4), ILLEGAL_INSTRCT,     // 0xA8 - 0xAF
    IN(bcs, rel, 2), IN(lda, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(ldy, zpx, 4), IN(lda, zpx, 4), IN(ldx, zpy, 4), ILLEGAL_INSTRCT,     // 0xB0 - 0xB7
    IN(clv, imp, 2), IN(lda, aby, 4), IN(tsx, imp, 2), ILLEGAL_INSTRCT, IN(ldy, abx, 4), IN(lda, abx, 4), IN(ldx, aby, 4), ILLEGAL_INSTRCT,     // 0xB8 - 0xBF
    IN(cpy, imm, 2), IN(cmp, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(cpy, zpg, 3), IN(cmp, zpg, 3), IN(dec, zpg, 5), ILLEGAL_INSTRCT,     // 0xC0 - 0xC7
    IN(iny, imp, 2), IN(cmp, imm, 2), IN(dex, imp, 2), ILLEGAL_INSTRCT, IN(cpy, abs, 4), IN(cmp, abs, 4), IN(dec, abs, 6), ILLEGAL_INSTRCT,     // 0xC8 - 0xCF
    IN(bne, rel, 2), IN(cmp, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(cmp, zpx, 4), IN(dec, zpx, 6), ILLEGAL_INSTRCT,     // 0xD0 - 0xD7
    IN(cld, imp, 2), IN(cmp, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(cmp, abx, 4), IN(dec, abx, 7), ILLEGAL_INSTRCT,     // 0xD8 - 0xDF
    IN(cpx, imm, 2), IN(sbc, idx, 6), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(cpx, zpg, 3), IN(sbc, zpg, 3), IN(inc, zpg, 5), ILLEGAL_INSTRCT,     // 0xE0 - 0xE7
    IN(inx, imp, 2), IN(sbc, imm, 2), IN(nop, imp, 2), ILLEGAL_INSTRCT, IN(cpx, abs, 4), IN(sbc, abs, 4), IN(inc, abs, 6), ILLEGAL_INSTRCT,     // 0xE8 - 0xEF
    IN(beq, rel, 2), IN(sbc, idy, 5), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(sbc, zpx, 4), IN(inc, zpx, 6), ILLEGAL_INSTRCT,     // 0xF0 - 0xF7
    IN(sed, imp, 2), IN(sbc, aby, 4), ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, ILLEGAL_INSTRCT, IN(sbc, abx, 4), IN(inc, abx, 7), ILLEGAL_INSTRCT,     // 0xF8 - 0xFF
};

void write(uint16_t addr, uint8_t data);
uint8_t read(uint16_t addr);
uint8_t fetch(_state* state);
uint8_t is_imp(_state* state);

void set_flag(_state* state, _flag flag, uint8_t value);
uint8_t get_flag(_state* state, _flag flag);

void branch(_state* state);

void reset(_state* state);
void irq(_state* state);
void nmi(_state* state);

void clock(_state* state);
