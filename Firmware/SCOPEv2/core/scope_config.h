#pragma once

#if defined(__LGT8F__) || defined(__LGT8FX8P__) || defined(LARDUINO_HSP)
#define IS_LGT8F 1
#else
#define IS_LGT8F 0
#endif

// 0 = Compile without support for the generator mode, which saves
// about 1.5KB of flash and 100 bytes of RAM. The generator mode will be hidden in the UI and inaccessible.
// This is recommended if you don't have compatible hardware or don't need the generator mode features.
// 1 = Compile with support for enabling the generator mode (requires compatible hardware)
#ifndef ENABLE_GEN_MODE
#define ENABLE_GEN_MODE 0
#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define V2_OLED_MOSI 9
#define V2_OLED_CLK 10
#define V2_OLED_DC 11
#define V2_OLED_CS 12
#define V2_OLED_RESET 13

#define V25_OLED_DC 9
#define V25_OLED_RESET 8
#define V25_OLED_MOSI 11
#define V25_OLED_CS 10
#define V25_OLED_CLK 13

#define V2_ENCODER_PIN_A 2
#define V2_ENCODER_PIN_B 4
#define V25_ENCODER_PIN_A A2
#define V25_ENCODER_PIN_B A3

#define IDENT_HW_PIN A7
#define BUTTON_PIN 5
#define FILTER_PIN 6
#define TRIGGER_PIN 7
#define OFFSET_PIN 3
#define ANALOG_INPUT_PIN 0

#define EEPROM_MAGIC_ADDR 0
#define ENCODER_DIR_ADDR 1
#define OLED_ROT_ADDR 2
#define MENUTIMER_DIR_ADDR 3
#define EEPROM_MODE_ADDR 5
#define EEPROM_PARAM_SELECT_ADDR 6
#define EEPROM_CAL_OFFSET_ADDR 18
#define EEPROM_CAL_GAIN_ADDR 19

#define EEPROM_MAGIC_VALUE 0xA9

#define MODE_LFO 1
#define MODE_WAVE 2
#define MODE_TUNER 3
#define MODE_GEN 4

#if ENABLE_GEN_MODE
#define NUM_MODES 4
#else
#define NUM_MODES 3
#endif

#define ADC_BUFFER_SIZE 256
#define GEN_NUM_FREQS 14
#define GEN_DC_MAX 50
