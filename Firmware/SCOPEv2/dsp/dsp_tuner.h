#pragma once

#include <Arduino.h>

void frequencyToNote(uint16_t freqX10, char *note, int8_t *octave, int8_t *cents);
float detectFrequencyZC();
