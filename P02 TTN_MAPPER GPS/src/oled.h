#ifndef OLED_H_
#define OLED_H_

#include "SSD1306.h" //https://github.com/ThingPulse/esp8266-oled-ssd1306

#define OLED_SDA    4  // GPIO21  -- OLED'S SDA
#define OLED_SCL    15 // GPIO22  -- OLED's SCL compartido con LED de placa! :(
#define OLED_RST    16 // GPIO16  -- OLED's RESET
#define OLED_ADDR   0x3c  // OLED's ADDRESS

void setupOLED(void);

extern SSD1306 display;

#endif