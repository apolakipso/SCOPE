#pragma once

#include <Arduino.h>

#include "../core/scope_config.h"

extern const uint16_t genFreqTableX10[] PROGMEM;
extern const char genWaveNames[][4] PROGMEM;

uint8_t sineWave(uint8_t idx);
uint8_t generateSample(uint8_t waveform, uint8_t idx);
uint32_t calcPhaseInc(uint8_t freqIdx, uint32_t sr);
