#pragma once

#include <Arduino.h>

void startADCSampling(uint16_t numSamples, uint8_t prescaler, uint8_t delaySkip);
void stopADCSampling();
uint8_t fastAnalogRead();
