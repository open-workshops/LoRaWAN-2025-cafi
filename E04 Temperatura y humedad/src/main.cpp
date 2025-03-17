/*******************************************************************************
  E06: Ejemplo de sensor de temperatura y humedad
  
  Este ejemplo permite leer datos de un sensor SHT30 e imprimirlos por serial
  - La placa lee el estado de un sensor SHT30 (temperatura y humedad)
  - Se imprimen los datos por serial

  Conexiones:
    Sensor de temperatura     Placa Heltec
    VIN                       3.3
    GND                       GND
    SCL                       PIN 15
    SDA                       PIN 4
  
 ******************************************************************************/

#include <Arduino.h>
#include "SHT85.h"

SHT30 sht;

int SDA_PIN = 41;
int SCL_PIN = 42;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicialización del sensor SHT30
  Wire.begin(SDA_PIN, SCL_PIN, 100000U);
  sht.begin(0x44);
}

void loop() {
  if (!sht.read())  // solicitamos medición de temperatura y humedad al sensor
  {
    // si ha habido un error, no continuamos
    Serial.print("Error leyendo el sensor: ");
    Serial.println(sht.getError());
    delay(10000);
  }

  Serial.print("Temperatura: ");
  Serial.print(sht.getTemperature());
  Serial.println("º C");

  Serial.print("Humedad: ");
  Serial.print(sht.getHumidity());
  Serial.println(" %");
  
  Serial.println();

  delay(4000);
}