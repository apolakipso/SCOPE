#include "ui_controls.h"

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../app/app_mode_manager.h"
#include "../app/storage_settings.h"
#include "ui_config_menu.h"

bool processUiControls()
{
    old_SW = SW;
    old_mode = mode;
    SW = (digitalRead(BUTTON_PIN) == LOW);

    static unsigned long bStart = 0;
    static bool isLP = false, hasSaved = false, hasConfig = false;

    if (SW && !old_SW)
    {
        bStart = millis();
        isLP = true;
        hasSaved = false;
        hasConfig = false;
    }
    if (!SW && old_SW)
        isLP = false;

    if (isLP && !hasSaved && (millis() - bStart >= 1000))
    {
        saveCurrentModeToRAM();
        saveAllSettings();
        hasSaved = true;
    }

    if (SW && !configMenuActive && !hasConfig && hasSaved && (millis() - bStart >= 3000))
    {
        configMenuActive = true;
        hasConfig = true;
        configMenuOption = 1;
        oldPosition = newPosition = encoder->read();
    }

    if (configMenuActive)
    {
        configMenu();
        return true;
    }

    newPosition = encoderDirection * encoder->read();
    if (old_SW == 0 && SW == 1 && param_select == param)
    {
        param_select = 0;
        hideTimer = millis();
    }
    else if (old_SW == 0 && SW == 1 && (param >= 1 && param <= 3))
    {
        param_select = param;
        hideTimer = millis();
    }

    newPosition = encoderDirection * encoder->read();
    int8_t enc = 0;
    if ((newPosition - 3) / 4 > oldPosition / 4)
    {
        oldPosition = newPosition;
        hideTimer = millis();
        enc = -1;
    }
    else if ((newPosition + 3) / 4 < oldPosition / 4)
    {
        oldPosition = newPosition;
        hideTimer = millis();
        enc = 1;
    }

    if (enc)
    {
        switch (param_select)
        {
        case 0:
        {
            uint8_t mx = (mode == MODE_TUNER) ? 1 : 3;
            param += enc;
            if (param < 1)
                param = mx;
            if (param > mx)
                param = 1;
        }
        break;
        case 1:
            mode += enc;
            if (mode < 1)
                mode = NUM_MODES;
            if (mode > NUM_MODES)
                mode = 1;
#if ENABLE_GEN_MODE
            if (mode == MODE_GEN && !genAvailable)
            {
                mode += enc;
                if (mode < 1)
                    mode = NUM_MODES;
                if (mode > NUM_MODES)
                    mode = 1;
            }
#endif
            break;
        case 2:
        {
            int8_t mn = 1, mx;
            switch (mode)
            {
            case MODE_LFO:
            case MODE_WAVE:
                mx = 8;
                break;
            case MODE_TUNER:
                mx = 1;
                break;
#if ENABLE_GEN_MODE
            case MODE_GEN:
                mx = 5;
                break;
#endif
            default:
                mx = 8;
                break;
            }
            param1 += enc;
            if (param1 < mn)
                param1 = mx;
            if (param1 > mx)
                param1 = mn;
#if ENABLE_GEN_MODE
            if (mode == MODE_GEN)
            {
                if (param1 == 5)
                {
                    if (param2 > GEN_DC_MAX)
                        param2 = GEN_DC_MAX / 2;
                }
                else
                {
                    if (param2 > GEN_NUM_FREQS || param2 == 0)
                        param2 = 4;
                }
            }
#endif
        }
        break;
        case 3:
        {
            int8_t mn, mx;
            switch (mode)
            {
            case MODE_LFO:
                mn = -6;
                mx = 10;
                break;
            case MODE_WAVE:
                mn = 1;
                mx = 6;
                break;
#if ENABLE_GEN_MODE
            case MODE_GEN:
                mn = (param1 == 5) ? 0 : 1;
                mx = (param1 == 5) ? GEN_DC_MAX : GEN_NUM_FREQS;
                break;
#endif
            default:
                mn = 1;
                mx = 1;
                break;
            }
            param2 += enc;
            if (param2 < mn)
                param2 = mx;
            if (param2 > mx)
                param2 = mn;
        }
        break;
        }
    }

    if (old_mode != mode)
    {
        byte ps = param_select;
        saveCurrentModeToRAM();
        setupMode(mode);
        display->clearDisplay();
        if (ps == 1)
            param_select = 1;
        hideTimer = millis();
    }

    return false;
}
