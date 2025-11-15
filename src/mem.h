#pragma once
#include "cpu.h"
#include <stdint.h>

void ram_write(_state* state, uint16_t addr, uint8_t data);
uint8_t ram_read(_state* state, uint16_t addr);
uint8_t rom_read(_state* state, uint16_t addr);
