/***************************************************************************
  E03: Ejemplo Sesnsor de puerta

 ****************************************************************************/
#include <Arduino.h>

// Definición de pines
#define DOOR_SENSOR 6
#define LED_RED 34
#define LED_GREEN 36

// Sensor normalmente cerrado
// Puerta cerrada -> contacto cerrado
#define DOOR_CLOSED LOW 
#define DOOR_OPEN HIGH

int lastStatus = DOOR_OPEN;

void setup()
{
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  
  Serial.begin(115200);
  delay(20000);
  Serial.println("Iniciando E03 Ejemplo Sensor puerta");
}

void loop()
{
  int status = digitalRead(DOOR_SENSOR);

  if (status != lastStatus)
  {
    lastStatus = status;

    if (status == DOOR_CLOSED)
    {
      Serial.println("Puerta CERRADA");
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
    }
    else 
    {
      Serial.println("Puerta ABIERTA");
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
    }

    // Delay para evitar rebotes. 
    delay(1000);
  }
}
