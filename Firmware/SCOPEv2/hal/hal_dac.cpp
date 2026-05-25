#include "hal_dac.h"

#include <Wire.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"

#if IS_LGT8F
#ifndef DAC0
#define DAC0 4
#endif
#ifndef ANALOG
#define ANALOG 2
#endif
#endif

void scanMCP4725()
{
    for (uint8_t addr = 0x60; addr <= 0x63; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0)
        {
            dacI2CAddr = addr;
            dacIsI2C = true;
            return;
        }
    }
    dacI2CAddr = 0;
    dacIsI2C = false;
}

void dacInit()
{
#if IS_LGT8F
    analogReference(DEFAULT);
    pinMode(DAC0, ANALOG);
    analogWrite(DAC0, 0);
    genSampleRate = 50000UL;
    genSamplePeriodUs = 20;
#else
    if (dacIsI2C)
    {
        Wire.beginTransmission(dacI2CAddr);
        Wire.write(0x40);
        Wire.write(0);
        Wire.write(0);
        Wire.endTransmission();
    }
    genSampleRate = 10000UL;
    genSamplePeriodUs = 100;
#endif
}

void dacStop()
{
#if IS_LGT8F
    analogWrite(DAC0, 0);
    pinMode(DAC0, INPUT);
#else
    if (dacIsI2C)
    {
        Wire.beginTransmission(dacI2CAddr);
        Wire.write(0x40);
        Wire.write(0);
        Wire.write(0);
        Wire.endTransmission();
    }
#endif
}

void dacWrite(uint8_t value)
{
    int16_t cal = ((int16_t)value * calGain) >> 7;
    cal += calOffset;
    if (cal < 0)
        cal = 0;
    if (cal > 255)
        cal = 255;
#if IS_LGT8F
    analogWrite(DAC0, (uint8_t)cal);
#else
    if (dacIsI2C)
    {
        uint16_t val12 = (uint16_t)cal << 4;
        Wire.beginTransmission(dacI2CAddr);
        Wire.write((uint8_t)(val12 >> 8));
        Wire.write((uint8_t)(val12 & 0xFF));
        Wire.endTransmission();
    }
#endif
}

uint8_t dcVoltageToDac(uint8_t param2_x10)
{
    return (uint8_t)((uint16_t)param2_x10 * 255 / GEN_DC_MAX);
}
