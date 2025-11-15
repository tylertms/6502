#pragma once
#include "main.h"

void bus_write(_state* state, uint16_t addr, uint8_t data);
uint8_t bus_read(_state* state, uint16_t addr);
uint8_t bus_fetch(_state* state);
