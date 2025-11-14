#include "cpu.h"
#include <stdlib.h>
#include <string.h>

int main() {
    _state* state = (_state*)calloc(1, sizeof(_state));

    const uint16_t rom_start = 0xC000;
    const uint16_t prog_len = 28;
    const uint8_t prog[] = {
        0xA2, 0x0A, 0x8E, 0x00,
        0x00, 0xA2, 0x03, 0x8E,
        0x01, 0x00, 0xAC, 0x00,
        0x00, 0xA9, 0x00, 0x18,
        0x6D, 0x01, 0x00, 0x88,
        0xD0, 0xFA, 0x8D, 0x02,
        0x00, 0xEA, 0xEA, 0xEA
    };

    memcpy(state->rom, prog, prog_len);

    state->rom[RST_VECTOR - rom_start] = 0x00;
    state->rom[RST_VECTOR - rom_start + 1] = 0x80;

    cpu_reset(state);

    while (!state->stop) {
        cpu_clock(state);
    }

    free(state);
    return 0;
}
