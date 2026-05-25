#include "ui_parameter_bar.h"

#include <string.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../dsp/dsp_generator.h"
#include "../dsp/util_format.h"

void drawParameterBar(bool showParams)
{
    if (!showParams)
        return;

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
    case MODE_TUNER:
        display->print(F("TUNE"));
        slot1Width = 24;
        break;
    case MODE_GEN:
        display->print(F("GEN"));
        break;
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
    case MODE_TUNER:
        display->print(F("ZC"));
        slot2Width = 12;
        break;
    case MODE_GEN:
    {
        char w[4];
        memcpy_P(w, genWaveNames[constrain(param1, 1, 5) - 1], 4);
        display->print(w);
    }
    break;
    }
    if (cursorNavActive && param == 2)
        display->drawFastHLine(36, 8, slot2Width, WHITE);

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
    else if (mode != MODE_TUNER)
    {
        display->setTextColor(param_select == 3 ? BLACK : WHITE, param_select == 3 ? WHITE : BLACK);
        display->setCursor(78, 0);
        display->print(mode == MODE_LFO ? F("O:") : F("R:"));
        display->print(param2);
        if (cursorNavActive && param == 3)
        {
            uint8_t slot3Width = (param2 < 0) ? 24 : 18;
            display->drawFastHLine(78, 8, slot3Width, WHITE);
        }
    }
}
