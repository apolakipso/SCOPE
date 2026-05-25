#include "mode_gen.h"

#include <string.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../dsp/dsp_generator.h"
#include "../hal/hal_dac.h"
#include "../ui/ui_parameter_bar.h"
#include "../dsp/util_format.h"

void runGeneratorMode(bool showParams)
{
    param = constrain(param, 1, 3);
    param1 = constrain(param1, 1, 5);

    static unsigned long lastDraw = 0;
    static unsigned long lastSample = 0;

    if (!genAvailable)
    {
        if (millis() - lastDraw >= 200)
        {
            lastDraw = millis();
            display->clearDisplay();
            display->setTextSize(1);
            display->setCursor(10, 24);
            display->print(F("GEN needs v2.5"));
            if (showParams)
                drawParameterBar(true);
            display->display();
        }
        return;
    }

    if (param1 != 5)
    {
        param2 = constrain(param2, 1, GEN_NUM_FREQS);
        genPhaseInc = calcPhaseInc(param2, genSampleRate);
    }
    else
    {
        param2 = constrain(param2, 0, GEN_DC_MAX);
        genPhaseInc = 0;
    }

    if (param1 == 5)
    {
        dacWrite(dcVoltageToDac(param2));
    }
    else
    {
        unsigned long nowComp = micros();
        unsigned long gap = nowComp - lastSample;
        if (gap > genSamplePeriodUs && gap < 50000UL && genPhaseInc > 0)
        {
            uint32_t missed = gap / genSamplePeriodUs;
            genPhase += missed * genPhaseInc;
            lastSample += missed * genSamplePeriodUs;
        }
        else if (gap >= 50000UL)
        {
            lastSample = nowComp;
        }

        unsigned long burstEnd = micros() + 4000;
        while ((long)(micros() - burstEnd) < 0)
        {
            unsigned long nowUs = micros();
            if (nowUs - lastSample >= genSamplePeriodUs)
            {
                lastSample += genSamplePeriodUs;
                dacWrite(generateSample(param1, genPhase >> 24));
                genPhase += genPhaseInc;
            }
        }
    }

    static bool genDispDone = false;
    if (param1 != 5 && !showParams && genDispDone)
        return;
    if (showParams)
        genDispDone = false;

    if (millis() - lastDraw < 80)
        return;
    lastDraw = millis();
    genDispDone = !showParams;

    display->clearDisplay();
    int yO = showParams ? 10 : 0;

    if (param1 == 5)
    {
        display->setTextSize(2);
        char vStr[8];
        fmtDec1(vStr, param2);
        int vW = strlen(vStr) * 12 + 12;
        display->setCursor((128 - vW) / 2, yO + 6);
        display->print(vStr);
        display->print('V');

        int barW = map(param2, 0, GEN_DC_MAX, 0, 100);
        int barY = yO + 28;
        display->drawRect(14, barY, 100, 8, WHITE);
        if (barW > 0)
            display->fillRect(14, barY, barW, 8, WHITE);

        display->setTextSize(1);
        display->setCursor(0, yO + 42);
        display->print(F("DAC:"));
        display->print(dcVoltageToDac(param2));
        display->setCursor(60, yO + 42);
        display->print(F("CAL:"));
        if (calOffset >= 0)
            display->print('+');
        display->print(calOffset);
    }
    else
    {
        int pH = 28, pY = yO + 2;
        for (int x = 0; x < 128; x++)
        {
            uint8_t idx = (uint8_t)((uint16_t)x * 512 / 128);
            int y = pY + pH - 1 - ((int)generateSample(param1, idx) * (pH - 1) / 255);
            display->drawPixel(x, constrain(y, pY, pY + pH - 1), WHITE);
        }

        uint16_t fX10 = pgm_read_word(&genFreqTableX10[constrain(param2, 1, GEN_NUM_FREQS) - 1]);
        char fStr[12];
        fmtFreq(fStr, fX10);
        display->setTextSize(1);
        display->setCursor((128 - strlen(fStr) * 6) / 2, yO + 34);
        display->print(fStr);

        char wn[4];
        memcpy_P(wn, genWaveNames[param1 - 1], 4);
        display->setCursor(0, yO + 46);
        display->print(wn);
        display->setCursor(60, yO + 46);
        display->print(F("CAL:"));
        if (calOffset >= 0)
            display->print('+');
        display->print(calOffset);
    }

    display->setTextSize(1);
    if (showParams)
        drawParameterBar(true);
    display->display();
}
