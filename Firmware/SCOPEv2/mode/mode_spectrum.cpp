#include "mode_spectrum.h"

#include <fix_fft.h>

#include "../core/scope_state.h"
#include "../hal/hal_adc.h"
#include "../ui/ui_parameter_bar.h"

void runSpectrumMode(bool showParams)
{
    param = constrain(param, 1, 3);
    param1 = constrain(param1, 1, 4);
    param2 = constrain(param2, 1, 8);

    static unsigned long lastUpdate = 0;
    static uint8_t spectrumState = 0;

    switch (spectrumState)
    {
    case 0:
        if (millis() - lastUpdate >= 50)
        {
            startADCSampling(128, 0x06, 0);
            spectrumState = 1;
        }
        break;

    case 1:
        if (adcBufferReady)
        {
            spectrumState = 2;
        }
        break;

    case 2:
    {
        int8_t *real = (int8_t *)buffer;
        int8_t *imag = (int8_t *)&buffer[128];

        for (uint8_t i = 0; i < 128; i++)
        {
            real[i] = (int8_t)((int16_t)buffer[i] - 128);
            imag[i] = 0;
        }

        fix_fft(real, imag, 7, 0);

        display->clearDisplay();
        for (uint8_t i = 0; i < 64; i++)
        {
            int16_t re = real[i];
            int16_t im = imag[i];
            int16_t level = abs(re) + abs(im);
            if (level >= (param2 * 2))
            {
                int boosted = level + ((int)i * (param1 - 1));
                boosted = constrain(boosted, 0, 63);
                display->fillRect(i * 2, 63 - boosted, 2, boosted, WHITE);
            }
        }

        if (showParams)
            drawParameterBar(true);
        lastUpdate = millis();
        spectrumState = 0;
    }
    break;
    }
}