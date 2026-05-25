#include "hal_display.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"

void initDisplay()
{
    if (isHWv25)
    {
        SPI.begin();
        SPI.setClockDivider(SPI_CLOCK_DIV2);
        display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT,
                                       &SPI, V25_OLED_DC, V25_OLED_RESET, V25_OLED_CS);
    }
    else
    {
        display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT,
                                       V2_OLED_MOSI, V2_OLED_CLK, V2_OLED_DC, V2_OLED_RESET, V2_OLED_CS);
    }
    display->begin(SSD1306_SWITCHCAPVCC);
    display->setRotation(oledRotation);
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(WHITE);
}
