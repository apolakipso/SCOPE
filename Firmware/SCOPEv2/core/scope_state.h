#pragma once

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_SSD1306.h>

#include "scope_config.h"
#include "scope_types.h"

struct ScopeState
{
    Adafruit_SSD1306 *display;
    bool isHWv25;
    Encoder *encoder;

    volatile uint16_t adcSampleIndex;
    volatile uint16_t adcTargetSamples;
    volatile bool adcBufferReady;
    volatile bool adcSampling;
    volatile uint8_t adcDelayCounter;
    volatile uint8_t adcDelayTarget;

    float smoothedFrequency;
    uint8_t tunerSampleRate;
    float sampleRateHz;

    uint8_t waveState;
    uint8_t tunerState;

    bool isLGT8F;

    uint32_t genPhase;
    uint32_t genPhaseInc;
    bool genAvailable;
    bool dacIsI2C;
    uint8_t dacI2CAddr;
    int8_t calOffset;
    uint8_t calGain;
    uint32_t genSampleRate;
    uint16_t genSamplePeriodUs;

    uint8_t buffer[ADC_BUFFER_SIZE];

    ModeSettings modeSettings[NUM_MODES];

    uint8_t mode;
    uint8_t old_mode;
    uint8_t param_select;
    uint8_t param;
    uint8_t param1;
    uint8_t param2;
    bool trig;
    bool old_trig;
    bool SW;
    bool old_SW;
    unsigned long hideTimer;
    bool hide;
    int rfrs;
    float oldPosition;
    float newPosition;
    bool configMenuActive;
    byte configMenuOption;
    unsigned int menuTimer;
    int encoderDirection;
    uint8_t oledRotation;
};

extern ScopeState gState;

extern Adafruit_SSD1306 *&display;
extern bool &isHWv25;
extern Encoder *&encoder;

extern volatile uint16_t &adcSampleIndex;
extern volatile uint16_t &adcTargetSamples;
extern volatile bool &adcBufferReady;
extern volatile bool &adcSampling;
extern volatile uint8_t &adcDelayCounter;
extern volatile uint8_t &adcDelayTarget;

extern float &smoothedFrequency;
extern uint8_t &tunerSampleRate;
extern float &sampleRateHz;
extern uint8_t &waveState;
extern uint8_t &tunerState;

extern bool &isLGT8F;

extern uint32_t &genPhase;
extern uint32_t &genPhaseInc;
extern bool &genAvailable;
extern bool &dacIsI2C;
extern uint8_t &dacI2CAddr;
extern int8_t &calOffset;
extern uint8_t &calGain;
extern uint32_t &genSampleRate;
extern uint16_t &genSamplePeriodUs;

extern uint8_t (&buffer)[ADC_BUFFER_SIZE];
extern ModeSettings (&modeSettings)[NUM_MODES];

extern uint8_t &mode;
extern uint8_t &old_mode;
extern uint8_t &param_select;
extern uint8_t &param;
extern uint8_t &param1;
extern uint8_t &param2;
extern bool &trig;
extern bool &old_trig;
extern bool &SW;
extern bool &old_SW;
extern unsigned long &hideTimer;
extern bool &hide;
extern int &rfrs;
extern float &oldPosition;
extern float &newPosition;
extern bool &configMenuActive;
extern byte &configMenuOption;
extern unsigned int &menuTimer;
extern int &encoderDirection;
extern uint8_t &oledRotation;
