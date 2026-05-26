#include "app_mode_manager.h"

#include <string.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../hal/hal_adc.h"
#if ENABLE_GEN_MODE
#include "../hal/hal_dac.h"
#include "../dsp/dsp_generator.h"
#endif

void setupMode(uint8_t m)
{
    stopADCSampling();
#if ENABLE_GEN_MODE
    if (genAvailable)
        dacStop();
#endif

    uint8_t idx = m - 1;
    if (idx < NUM_MODES)
    {
        param_select = modeSettings[idx].param_select;
        param1 = modeSettings[idx].param1;
        param2 = modeSettings[idx].param2;
    }

    switch (m)
    {
    case MODE_LFO:
        pinMode(FILTER_PIN, INPUT);
        analogWrite(OFFSET_PIN, 0);
        ADCSRA = (ADCSRA & 0xF8) | 0x04;
        break;
    case MODE_WAVE:
        analogWrite(OFFSET_PIN, 127);
        pinMode(FILTER_PIN, INPUT);
        break;
    case MODE_SPECTRUM:
        analogWrite(OFFSET_PIN, 127);
        pinMode(FILTER_PIN, INPUT);
        break;
    case MODE_TUNER:
        analogWrite(OFFSET_PIN, 127);
        pinMode(FILTER_PIN, INPUT);
        smoothedFrequency = 0;
        tunerSampleRate = 2;
        break;
#if ENABLE_GEN_MODE
    case MODE_GEN:
        analogWrite(OFFSET_PIN, 0);
        pinMode(FILTER_PIN, INPUT);
        dacInit();
        genPhase = 0;
        if (param1 != 5)
        {
            genPhaseInc = calcPhaseInc(param2, genSampleRate);
        }
        else
        {
            genPhaseInc = 0;
        }
        break;
#endif
    }

    memset(buffer, 0, sizeof(buffer));
    waveState = 0;
    tunerState = 0;
    rfrs = 0;
}
