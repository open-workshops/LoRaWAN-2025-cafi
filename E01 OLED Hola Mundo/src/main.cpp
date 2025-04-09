/*******************************************************************************
  E01: Ejemplo Hola Mundo pantalla OLED
  
  Este ejemplo permite comprobar el funcionamiento de la pantalla.
  -En la pantalla se muestra el texto Hola Mundo
 ******************************************************************************/
#include <Arduino.h>
#include "SSD1306Wire.h"

#define OLED_ADDR   0x3c    // OLED's ADDRESS

SSD1306Wire display(OLED_ADDR, SDA_OLED, SCL_OLED);

void setup() {
  Serial.begin(115200);

  // Activamos Vext (alimentación externa conectada a OLED)
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);

  // Reseteamos la OLED
  pinMode(RST_OLED,OUTPUT);
  digitalWrite(RST_OLED, LOW);     
  delay(50);
  digitalWrite(RST_OLED, HIGH);   

  Serial.println("Iniciando pantalla...");
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.display();
}

void loop() {
  // La pantalla mide 128 de ancho por 64 de alto
  // El origen está en la parte superior izq
  // centramos primero horizontalmente dividiendo el ancho a la mitad
  // centramos verticalmente dividiendo el alto a la mitad
  // y restando la mitad del tamaño de la letra
  display.clear();
  display.drawString(128/2, 64/2-16/2, "Hola Mundo");
  display.display(); //esta función es la que escribe lo configurado
  delay(1000);

  display.clear();
  display.drawString(128/2, 64/2-16/2, "Hello World");
  display.display();
  delay(1000);
}
