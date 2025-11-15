#pragma once
#include "main.h"
#include <stdint.h>

uint8_t ppu_read(_state* state, uint16_t addr);
void ppu_write(_state* state, uint16_t addr, uint8_t data);
