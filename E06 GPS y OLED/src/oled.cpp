#include <Arduino.h>
#include <Wire.h> 
#include "SSD1306Wire.h"  
#include "oled.h"

SSD1306Wire display(OLED_ADDR, OLED_SDA, OLED_SCL); // Objeto pantalla OLED

void setupOLED(void)
{
  // Activamos Vext (alimentación externa conectada a OLED)
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);

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