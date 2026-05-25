#include "mode_wave.h"

#include "../core/scope_state.h"
#include "../hal/hal_adc.h"
#include "../ui/ui_parameter_bar.h"

void runWaveMode(bool showParams)
{
    param = constrain(param, 1, 3);
    param1 = constrain(param1, 1, 8);
    param2 = constrain(param2, 1, 6);

    static unsigned long lastUp = 0, stateStart = 0;
    unsigned long interval = 20UL + (param2 - 1) * 10UL;

    switch (waveState)
    {
    case 0:
        if (millis() - lastUp >= interval)
        {
            uint8_t ps, ds;
            if (param1 <= 5)
            {
                ps = 0x05;
                ds = (6 - param1) * 2;
            }
            else
            {
                ps = 0x06;
                ds = (param1 - 5) * 4;
            }
            startADCSampling(128, ps, ds);
            waveState = 1;
            stateStart = millis();
        }
        break;
    case 1:
        if (adcBufferReady)
        {
            lastUp = millis();
            waveState = 2;
        }
        else if (millis() - stateStart > 100)
        {
            stopADCSampling();
            waveState = 0;
        }
        break;
    case 2:
        display->clearDisplay();
        for (uint8_t i = 0; i < 128; i++)
            buffer[i] >>= 2;
        for (int i = 1; i < 127; i++)
        {
            display->drawLine(127 - i, buffer[i - 1], 127 - (i + 1), buffer[i], WHITE);
        }
        if (showParams)
            drawParameterBar(true);
        waveState = 0;
        break;
    }
}
