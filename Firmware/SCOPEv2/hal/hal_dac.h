#pragma once

#include <Arduino.h>

void scanMCP4725();
void dacInit();
void dacStop();
void dacWrite(uint8_t value);
uint8_t dcVoltageToDac(uint8_t param2_x10);
