#pragma once

#include <Arduino.h>

char *fmtInt(char *buf, int16_t val);
char *fmtDec1(char *buf, int16_t valX10);
void fmtDec2(char *buf, uint16_t vx100);
void fmtFreq(char *buf, uint16_t freqX10);
