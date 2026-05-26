#include "ui_parameter_bar.h"

#include <string.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#if ENABLE_GEN_MODE
#include "../dsp/dsp_generator.h"
#endif
#include "../dsp/util_format.h"

static uint8_t getActiveSlot()
{
    return (param_select == 0) ? param : param_select;
}

static bool shouldShowLargeOverlay(bool showParams)
{
    if (!showParams)
        return false;

    static bool initialized = false;
    static uint8_t prevMode = 0;
    static uint8_t prevParam1 = 0;
    static uint8_t prevParam2 = 0;
    static unsigned long lastValueChangeMs = 0;

    if (!initialized)
    {
        prevMode = mode;
        prevParam1 = param1;
        prevParam2 = param2;
        initialized = true;
    }

    if (prevMode != mode || prevParam1 != param1 || prevParam2 != param2)
    {
        prevMode = mode;
        prevParam1 = param1;
        prevParam2 = param2;
        lastValueChangeMs = millis();
    }

    return lastValueChangeMs > 0 && (millis() - lastValueChangeMs < 1000UL);
}

static void drawLargeParameterPanel()
{
    const uint8_t activeSlot = getActiveSlot();

    if (activeSlot == 1)
    {
        display->fillRect(0, 8, 128, 56, BLACK);
        display->setTextSize(3);
        display->setTextColor(WHITE);
        display->setCursor(6, 20);

        switch (mode)
        {
        case MODE_LFO:
            display->print(F(" LFO"));
            display->write((uint8_t)2);
            break;
        case MODE_WAVE:
            display->print(F(" WAVE"));
            break;
        case MODE_SPECTRUM:
            display->print(F(" SPEC"));
            break;
        case MODE_TUNER:
            display->print(F(" TUNE"));
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
            display->print(F(" GEN"));
            break;
#endif
        }

        display->setTextSize(1);
        return;
    }

    display->setTextSize(2);
    display->setTextColor(WHITE);
    display->setCursor(6, 22);
    bool labelHasPadding = false;

    if (activeSlot == 2)
    {
        switch (mode)
        {
        case MODE_LFO:
        case MODE_WAVE:
            display->print(F("  TIME "));
            labelHasPadding = true;
            break;
        case MODE_SPECTRUM:
            display->print(F(" HIGH"));
            break;
        case MODE_TUNER:
            display->print(F("ZERO X"));
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
            display->print(F("WAVE"));
            break;
#endif
        }
    }
    else
    {
        switch (mode)
        {
        case MODE_LFO:
            display->print(F(" OFFSET "));
            labelHasPadding = true;
            break;
        case MODE_WAVE:
            display->print(F(" RANGE "));
            labelHasPadding = true;
            break;
        case MODE_SPECTRUM:
            display->print(F(" FILTER"));
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
            display->print(param1 == 5 ? F("LEVEL") : F("FREQ"));
            break;
#endif
        }
    }

    if (!labelHasPadding)
        display->print(' ');

    if (activeSlot == 2)
    {
        switch (mode)
        {
        case MODE_LFO:
        case MODE_WAVE:
            display->print(param1);
            break;
        case MODE_SPECTRUM:
            display->print(param1);
            break;
        case MODE_TUNER:
            display->print(F("ON"));
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
        {
            char w[4];
            memcpy_P(w, genWaveNames[constrain(param1, 1, 5) - 1], 4);
            display->print(w);
        }
        break;
#endif
        }
    }
    else
    {
        switch (mode)
        {
        case MODE_LFO:
        case MODE_WAVE:
            display->print(param2);
            break;
        case MODE_SPECTRUM:
            display->print(param2);
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
            if (param1 == 5)
            {
                char v[6];
                fmtDec1(v, param2);
                display->print(v);
                display->print('V');
            }
            else
            {
                char f[10];
                uint16_t fX10 = pgm_read_word(&genFreqTableX10[constrain(param2, 1, GEN_NUM_FREQS) - 1]);
                fmtFreq(f, fX10);
                display->print(f);
            }
            break;
#endif
        }
    }
    display->setTextSize(1);
}

void drawParameterBar(bool showParams)
{
    if (!showParams)
        return;

    bool showLargeOverlay = shouldShowLargeOverlay(showParams);

    display->setTextSize(1);
    bool cursorNavActive = (param_select == 0);

    display->setTextColor(param_select == 1 ? BLACK : WHITE, param_select == 1 ? WHITE : BLACK);
    display->setCursor(0, 0);
    uint8_t slot1Width = 18;
    switch (mode)
    {
    case MODE_LFO:
        display->print(F("LFO"));
        break;
    case MODE_WAVE:
        display->print(F("WAVE"));
        slot1Width = 24;
        break;
    case MODE_SPECTRUM:
        display->print(F("SPEC"));
        slot1Width = 24;
        break;
    case MODE_TUNER:
        display->print(F("TUNE"));
        slot1Width = 24;
        break;
#if ENABLE_GEN_MODE
    case MODE_GEN:
        display->print(F("GEN"));
        break;
#endif
    }
    if (cursorNavActive && param == 1)
        display->drawFastHLine(0, 8, slot1Width, WHITE);

    display->setTextColor(param_select == 2 ? BLACK : WHITE, param_select == 2 ? WHITE : BLACK);
    display->setCursor(36, 0);
    uint8_t slot2Width = 18;
    switch (mode)
    {
    case MODE_LFO:
    case MODE_WAVE:
        display->print(F("T:"));
        display->print(param1);
        break;
    case MODE_SPECTRUM:
        display->print(F("H:"));
        display->print(param1);
        break;
    case MODE_TUNER:
        display->print(F("ZC"));
        slot2Width = 12;
        break;
#if ENABLE_GEN_MODE
    case MODE_GEN:
    {
        char w[4];
        memcpy_P(w, genWaveNames[constrain(param1, 1, 5) - 1], 4);
        display->print(w);
    }
    break;
#endif
    }
    if (cursorNavActive && param == 2)
        display->drawFastHLine(36, 8, slot2Width, WHITE);

#if ENABLE_GEN_MODE
    if (mode == MODE_GEN)
    {
        display->setTextColor(param_select == 3 ? BLACK : WHITE, param_select == 3 ? WHITE : BLACK);
        display->setCursor(66, 0);
        uint8_t slot3Width;
        if (param1 == 5)
        {
            char v[6];
            fmtDec1(v, param2);
            display->print(v);
            display->print('V');
            slot3Width = (strlen(v) + 1) * 6;
        }
        else
        {
            uint16_t fX10 = pgm_read_word(&genFreqTableX10[constrain(param2, 1, GEN_NUM_FREQS) - 1]);
            char f[10];
            fmtFreq(f, fX10);
            display->print(f);
            slot3Width = strlen(f) * 6;
        }
        if (cursorNavActive && param == 3)
            display->drawFastHLine(66, 8, slot3Width, WHITE);
    }
    else
#endif
        if (mode != MODE_TUNER)
    {
        display->setTextColor(param_select == 3 ? BLACK : WHITE, param_select == 3 ? WHITE : BLACK);
        display->setCursor(78, 0);
        if (mode == MODE_LFO)
            display->print(F("O:"));
        else if (mode == MODE_WAVE)
            display->print(F("R:"));
        else
            display->print(F("F:"));
        display->print(param2);
        if (cursorNavActive && param == 3)
        {
            uint8_t slot3Width = (param2 < 0) ? 24 : 18;
            display->drawFastHLine(78, 8, slot3Width, WHITE);
        }
    }

    if (showLargeOverlay)
        drawLargeParameterPanel();
}
