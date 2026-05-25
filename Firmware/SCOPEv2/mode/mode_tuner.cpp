#include "mode_tuner.h"

#include <string.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../hal/hal_adc.h"
#include "../dsp/dsp_tuner.h"
#include "../dsp/util_format.h"
#include "../ui/ui_parameter_bar.h"

void runTunerMode(bool showParams)
{
    param = 1;

    static unsigned long lastUp = 0, stateStart = 0;
    static float lastValid = 0;

    switch (tunerState)
    {
    case 0:
        if (millis() - lastUp >= 40)
        {
            uint8_t ps, ds;
            if (lastValid < 80 || tunerSampleRate == 1)
            {
                ps = 0x07;
                ds = 1;
                tunerSampleRate = 1;
                sampleRateHz = IS_LGT8F ? 5682.0f : 4808.0f;
            }
            else if (lastValid < 200 || tunerSampleRate == 2)
            {
                ps = 0x07;
                ds = 0;
                tunerSampleRate = 2;
                sampleRateHz = IS_LGT8F ? 11364.0f : 9615.0f;
            }
            else
            {
                ps = 0x06;
                ds = 0;
                tunerSampleRate = 3;
                sampleRateHz = IS_LGT8F ? 22727.0f : 19230.0f;
            }
            startADCSampling(256, ps, ds);
            tunerState = 1;
            stateStart = millis();
        }
        break;
    case 1:
        if (adcBufferReady)
        {
            tunerState = 2;
        }
        else if (millis() - stateStart > 200)
        {
            stopADCSampling();
            tunerState = 0;
        }
        break;
    case 2:
    {
        float raw = detectFrequencyZC();
        if (raw > 15 && raw < 5000)
        {
            lastValid = raw;
            if (smoothedFrequency < 10)
            {
                smoothedFrequency = raw;
            }
            else
            {
                float a = (abs(raw - smoothedFrequency) / smoothedFrequency > 0.1f) ? 0.5f : 0.35f;
                smoothedFrequency = smoothedFrequency * (1 - a) + raw * a;
            }
            tunerSampleRate = (smoothedFrequency < 60) ? 1 : (smoothedFrequency < 150) ? 2
                                                                                       : 3;
        }
        else
        {
            smoothedFrequency *= 0.85f;
            if (smoothedFrequency < 15)
            {
                smoothedFrequency = 0;
                tunerSampleRate = 2;
            }
        }
        lastUp = millis();
        tunerState = 0;
    }
    break;
    }

    static unsigned long lastDraw = 0;
    if (millis() - lastDraw < 80)
        return;
    lastDraw = millis();

    display->clearDisplay();
    int yO = showParams ? 10 : 0;

    if (smoothedFrequency > 15)
    {
        uint16_t fx10 = (uint16_t)(smoothedFrequency * 10.0f);
        char note[3];
        int8_t oct, cents;
        frequencyToNote(fx10, note, &oct, &cents);

        display->setTextSize(2);
        int nW = (strlen(note) * 12) + 12;
        display->setCursor((128 - nW) / 2, yO + 4);
        display->print(note);
        display->print((int)oct);

        display->setTextSize(1);
        if (cents < 0)
        {
            display->setCursor(4, yO + 8);
            display->print(cents);
            display->print('c');
        }
        else if (cents > 0)
        {
            display->setCursor(cents < 10 ? 110 : 104, yO + 8);
            display->print('+');
            display->print(cents);
            display->print('c');
        }
        else
        {
            display->setCursor(110, yO + 8);
            display->print(F("OK"));
        }

        char hz[12];
        fmtFreq(hz, fx10);
        display->setCursor((128 - strlen(hz) * 6) / 2, yO + 24);
        display->print(hz);

        int wY = showParams ? 36 : 34;
        for (uint8_t i = 1; i < 127; i++)
        {
            int y = constrain(wY + 11 - ((buffer[i] * 12) >> 8), wY, wY + 11);
            display->drawPixel(i, y, WHITE);
        }

        int bY = showParams ? 52 : 50;
        display->drawRect(14, bY, 100, 6, WHITE);
        display->drawFastVLine(64, bY - 2, 10, WHITE);
        display->fillRect(62 + constrain(cents, -50, 50), bY + 1, 5, 4, WHITE);
    }
    else
    {
        display->setTextSize(2);
        display->setCursor(44, yO + 8);
        display->print(F("---"));
        display->setTextSize(1);
        display->setCursor(34, yO + 26);
        display->print(F("No signal"));
        int bY = showParams ? 52 : 50;
        display->drawRect(14, bY, 100, 6, WHITE);
        display->drawFastVLine(64, bY - 2, 10, WHITE);
    }

    display->setTextSize(1);
    if (showParams)
        drawParameterBar(true);
}
