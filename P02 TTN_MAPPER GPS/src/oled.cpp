#include <Arduino.h>
#include "SSD1306.h" //https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "oled.h"

SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL); // Objeto pantalla OLED

void setupOLED(void)
{
  pinMode(OLED_RST,OUTPUT);
  digitalWrite(OLED_RST, LOW);  // Ponemos a 0 para resetear OLED
  delay(50);
  digitalWrite(OLED_RST, HIGH); // Liberamos el reset poniéndolo a 1

  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.display();
}