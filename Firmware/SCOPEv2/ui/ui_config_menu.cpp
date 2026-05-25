#include "ui_config_menu.h"

#include <EEPROM.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"
#include "../app/storage_settings.h"

void configMenu()
{
    int newDir = encoderDirection;
    newPosition = encoder->read();
    const uint8_t NC = 5;

    if (newPosition > oldPosition + 3)
    {
        oldPosition = newPosition;
        switch (configMenuOption)
        {
        case 1:
            newDir = 1;
            break;
        case 2:
            if (menuTimer > 1)
                menuTimer--;
            break;
        case 3:
            oledRotation = 0;
            display->setRotation(0);
            break;
        case 4:
            if (calOffset > -50)
                calOffset--;
            break;
        case 5:
            if (calGain > 32)
                calGain--;
            break;
        }
    }
    else if (newPosition < oldPosition - 3)
    {
        oldPosition = newPosition;
        switch (configMenuOption)
        {
        case 1:
            newDir = -1;
            break;
        case 2:
            if (menuTimer < 60)
                menuTimer++;
            break;
        case 3:
            oledRotation = 2;
            display->setRotation(2);
            break;
        case 4:
            if (calOffset < 50)
                calOffset++;
            break;
        case 5:
            if (calGain < 255)
                calGain++;
            break;
        }
    }

    if (newDir != encoderDirection)
        encoderDirection = newDir;
    if (old_SW == 0 && SW == 1)
        configMenuOption = (configMenuOption % NC) + 1;

    static unsigned long holdStart = 0;
    if (SW && !old_SW)
        holdStart = millis();
    if (SW && holdStart > 0 && (millis() - holdStart >= 2000))
    {
        EEPROM.put(ENCODER_DIR_ADDR, encoderDirection);
        EEPROM.write(OLED_ROT_ADDR, oledRotation);
        EEPROM.put(MENUTIMER_DIR_ADDR, (uint8_t)menuTimer);
        saveCurrentModeToRAM();
        saveAllSettings();
        display->clearDisplay();
        display->setTextColor(WHITE);
        display->setCursor(16, 25);
        display->print(F("SETTINGS SAVED"));
        display->display();
        delay(800);
        configMenuActive = false;
        holdStart = 0;
        oldPosition = newPosition = encoder->read() * encoderDirection;
        return;
    }
    if (!SW)
        holdStart = 0;

    display->clearDisplay();
    display->setCursor(0, 0);
    display->setTextColor(WHITE);
    display->println(F("SETTINGS"));

    display->setTextColor(configMenuOption == 1 ? BLACK : WHITE, configMenuOption == 1 ? WHITE : BLACK);
    display->print(F("Enc: "));
    display->setTextColor(WHITE);
    display->println(encoderDirection == 1 ? F("Norm") : F("Rev"));

    display->setTextColor(configMenuOption == 2 ? BLACK : WHITE, configMenuOption == 2 ? WHITE : BLACK);
    display->print(F("Timer: "));
    display->setTextColor(WHITE);
    display->print(menuTimer);
    display->println('s');

    display->setTextColor(configMenuOption == 3 ? BLACK : WHITE, configMenuOption == 3 ? WHITE : BLACK);
    display->print(F("OLED: "));
    display->setTextColor(WHITE);
    display->println(oledRotation == 0 ? F("0") : F("180"));

    display->setTextColor(configMenuOption == 4 ? BLACK : WHITE, configMenuOption == 4 ? WHITE : BLACK);
    display->print(F("DAC Ofs: "));
    display->setTextColor(WHITE);
    if (calOffset >= 0)
        display->print('+');
    display->println(calOffset);

    display->setTextColor(configMenuOption == 5 ? BLACK : WHITE, configMenuOption == 5 ? WHITE : BLACK);
    display->print(F("DAC Gn: "));
    display->setTextColor(WHITE);
    uint16_t gainPct = ((uint16_t)calGain * 100) / 128;
    display->print(gainPct);
    display->println('%');

    display->setCursor(4, 56);
    display->print(F("Hold 2s "));
    display->print(isHWv25 ? F("v2.5") : F("v2"));
    display->print(isLGT8F ? F(" LGT") : F(" 328"));
    display->display();
}
