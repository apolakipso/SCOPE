#include "mode_lfo.h"

#include <string.h>

#include "../core/scope_state.h"
#include "../core/scope_config.h"
#include "../hal/hal_adc.h"
#include "../ui/ui_parameter_bar.h"

void runLFOMode(bool showParams)
{
    param = constrain(param, 1, 3);
    param1 = constrain(param1, 1, 8);
    param2 = constrain(param2, -6, 10);

    old_trig = trig;
    trig = digitalRead(TRIGGER_PIN);

    static bool frozen = false;
    static unsigned long frozenAt = 0;

    if (!old_trig && trig)
    {
        frozen = true;
        frozenAt = millis();
    }
    if (frozen && (millis() - frozenAt > 3000))
        frozen = false;

    if (!frozen)
    {
        uint8_t s = fastAnalogRead() >> 2;
        memmove(&buffer[1], &buffer[0], 127);
        buffer[0] = s;
    }

    static unsigned long lastDraw = 0;
    if (millis() - lastDraw >= 30)
    {
        lastDraw = millis();
        display->clearDisplay();

        int step = (9 - param1), voff = (param2 - 1) * 6, segs = 126 / step;
        for (int i = 0; i < segs; i++)
        {
            int x1 = 127 - (i * step), y1 = constrain(buffer[i] + voff, 0, 63);
            int x2 = 127 - ((i + 1) * step), y2 = constrain(buffer[i + 1] + voff, 0, 63);
            display->drawLine(x1, y1, x2, y2, WHITE);
        }

        if (frozen)
        {
            display->setCursor(0, 56);
            display->print(F("TRIG"));
        }

        if (showParams)
            drawParameterBar(true);
    }
}
