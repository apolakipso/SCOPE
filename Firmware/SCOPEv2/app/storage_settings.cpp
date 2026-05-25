#include "storage_settings.h"

#include <EEPROM.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"

void resetEEPROMDefaults()
{
    EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    EEPROM.update(ENCODER_DIR_ADDR, 1);
    EEPROM.update(OLED_ROT_ADDR, 0);
    EEPROM.update(MENUTIMER_DIR_ADDR, 5);
    EEPROM.update(EEPROM_MODE_ADDR, MODE_LFO);
    for (uint8_t m = 0; m < NUM_MODES; m++)
    {
        int ba = EEPROM_PARAM_SELECT_ADDR + (m * 3);
        EEPROM.update(ba, 0);
        EEPROM.update(ba + 1, 2);
        EEPROM.update(ba + 2, 1);
    }
    EEPROM.update(EEPROM_CAL_OFFSET_ADDR, 0);
    EEPROM.update(EEPROM_CAL_GAIN_ADDR, 128);
}

void loadAllSettings()
{
    if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VALUE)
        resetEEPROMDefaults();

    calOffset = (int8_t)EEPROM.read(EEPROM_CAL_OFFSET_ADDR);
    calGain = EEPROM.read(EEPROM_CAL_GAIN_ADDR);
    if (calGain < 32 || calGain > 255)
        calGain = 128;

    for (uint8_t m = 0; m < NUM_MODES; m++)
    {
        int ba = EEPROM_PARAM_SELECT_ADDR + (m * 3);
        modeSettings[m].param_select = EEPROM.read(ba);
        modeSettings[m].param1 = EEPROM.read(ba + 1);
        modeSettings[m].param2 = EEPROM.read(ba + 2);
        if (modeSettings[m].param_select > 3)
            modeSettings[m].param_select = 0;

        switch (m + 1)
        {
        case MODE_LFO:
            modeSettings[m].param1 = constrain(modeSettings[m].param1, 1, 8);
            modeSettings[m].param2 = constrain(modeSettings[m].param2, -6, 10);
            if (!modeSettings[m].param1)
                modeSettings[m].param1 = 4;
            if (!modeSettings[m].param2)
                modeSettings[m].param2 = 1;
            break;
        case MODE_WAVE:
            modeSettings[m].param1 = constrain(modeSettings[m].param1, 1, 8);
            modeSettings[m].param2 = constrain(modeSettings[m].param2, 1, 6);
            if (!modeSettings[m].param1)
                modeSettings[m].param1 = 8;
            if (!modeSettings[m].param2)
                modeSettings[m].param2 = 1;
            break;
        case MODE_TUNER:
            modeSettings[m].param1 = 1;
            modeSettings[m].param2 = 1;
            break;
#if ENABLE_GEN_MODE
        case MODE_GEN:
            modeSettings[m].param1 = constrain(modeSettings[m].param1, 1, 5);
            if (!modeSettings[m].param1)
                modeSettings[m].param1 = 1;
            if (modeSettings[m].param1 == 5)
            {
                modeSettings[m].param2 = constrain(modeSettings[m].param2, 0, GEN_DC_MAX);
            }
            else
            {
                modeSettings[m].param2 = constrain(modeSettings[m].param2, 1, GEN_NUM_FREQS);
                if (!modeSettings[m].param2)
                    modeSettings[m].param2 = 4;
            }
            break;
#endif
        }
    }
}

void saveCurrentModeToRAM()
{
    uint8_t idx = mode - 1;
    if (idx < NUM_MODES)
    {
        modeSettings[idx].param_select = param_select;
        modeSettings[idx].param1 = param1;
        modeSettings[idx].param2 = param2;
    }
}

void saveAllSettings()
{
    EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    EEPROM.update(EEPROM_MODE_ADDR, mode);
    for (uint8_t m = 0; m < NUM_MODES; m++)
    {
        int ba = EEPROM_PARAM_SELECT_ADDR + (m * 3);
        EEPROM.update(ba, modeSettings[m].param_select);
        EEPROM.update(ba + 1, modeSettings[m].param1);
        EEPROM.update(ba + 2, modeSettings[m].param2);
    }
    EEPROM.update(EEPROM_CAL_OFFSET_ADDR, (uint8_t)calOffset);
    EEPROM.update(EEPROM_CAL_GAIN_ADDR, calGain);

    display->fillRect(0, 54, 128, 10, WHITE);
    display->setTextColor(BLACK);
    display->setCursor(4, 55);
    display->print(F("SETTINGS SAVED"));
    display->display();
    delay(500);
}
