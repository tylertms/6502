#pragma once
#include "cpu.h"
#include <stdint.h>

void mem_write(_state* state, uint16_t addr, uint8_t data);
uint8_t mem_read(_state* state, uint16_t addr);
uint8_t mem_fetch(_state* state);
