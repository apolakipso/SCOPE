#include "hal_hw.h"

#if ENABLE_GEN_MODE
#include <Wire.h>
#endif
#include <Encoder.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#if ENABLE_GEN_MODE
#include "hal_dac.h"
#endif

void detectHardware()
{
#if IS_LGT8F
    isLGT8F = true;
#else
    isLGT8F = false;
#endif
    isHWv25 = (analogRead(IDENT_HW_PIN) < 100);

#if ENABLE_GEN_MODE
#if IS_LGT8F
    genAvailable = isHWv25;
#else
    if (isHWv25)
    {
        Wire.begin();
        Wire.setClock(400000UL);
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);
        scanMCP4725();
        genAvailable = dacIsI2C;
    }
    else
    {
        genAvailable = false;
    }
#endif
#else
    genAvailable = false;
#endif

    encoder = isHWv25 ? new Encoder(V25_ENCODER_PIN_A, V25_ENCODER_PIN_B)
                      : new Encoder(V2_ENCODER_PIN_A, V2_ENCODER_PIN_B);
}
