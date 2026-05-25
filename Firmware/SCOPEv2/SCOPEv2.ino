/**
 * @file SCOPEv2.ino
 * @author Modulove
 * @brief Eurorack scope + Tuner + Function Generator
 * @version 3.4.15
 * @date 2026-05-25
 */

#include <EEPROM.h>

#include "core/scope_config.h"
#include "core/scope_state.h"

#include "hal/hal_hw.h"
#include "hal/hal_display.h"
#include "app/storage_settings.h"
#include "ui/ui_controls.h"
#include "app/app_mode_manager.h"

#include "mode/mode_lfo.h"
#include "mode/mode_wave.h"
#include "mode/mode_tuner.h"
#include "mode/mode_gen.h"

static void drawBootSplash()
{
  display->clearDisplay();
  display->setTextSize(2);
  display->setCursor(16, 10);
  display->print(F("SCOPE"));

  display->setTextSize(1);
  display->setCursor(16, 32);
  display->print(F("Modulove v3.4.15"));

  display->setCursor(0, 56);
  display->print(isHWv25 ? F("v2.5") : F("v2"));
  display->print(isLGT8F ? F(" LGT") : F(" 328"));
  if (genAvailable)
    display->print(dacIsI2C ? F(" I2C") : F(" DAC"));
  display->display();
  delay(800);
}

void setup()
{
  detectHardware();

  EEPROM.get(ENCODER_DIR_ADDR, encoderDirection);
  if (encoderDirection != 1 && encoderDirection != -1)
    encoderDirection = 1;

  oledRotation = EEPROM.read(OLED_ROT_ADDR);
  if (!(oledRotation == 0 || oledRotation == 2))
    oledRotation = 0;

  EEPROM.get(MENUTIMER_DIR_ADDR, menuTimer);
  if (menuTimer < 1 || menuTimer > 60)
    menuTimer = 5;

  uint8_t lastMode = EEPROM.read(EEPROM_MODE_ADDR);
  mode = (lastMode >= MODE_LFO && lastMode <= MODE_GEN) ? lastMode : MODE_LFO;
  if (mode == MODE_GEN && !genAvailable)
    mode = MODE_LFO;

  initDisplay();
  drawBootSplash();

  pinMode(OFFSET_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(FILTER_PIN, INPUT);
  pinMode(TRIGGER_PIN, INPUT);

  TCCR2B = (TCCR2B & B11111000) | B00000001;

  loadAllSettings();
  setupMode(mode);
}

void loop()
{
  if (processUiControls())
    return;

  hide = (millis() - hideTimer >= (menuTimer * 1000UL));
  bool showParams = !hide;

  switch (mode)
  {
  case MODE_LFO:
    runLFOMode(showParams);
    break;
  case MODE_WAVE:
    runWaveMode(showParams);
    break;
  case MODE_TUNER:
    runTunerMode(showParams);
    break;
  case MODE_GEN:
    runGeneratorMode(showParams);
    return;
  }

  display->display();
}
