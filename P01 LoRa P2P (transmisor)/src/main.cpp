/*******************************************************************************
  P1: Práctica de LoRa P2P (Transmisor)
  
  Este ejemplo permite enviar tramas de LoRa a bajo nivel.
 ******************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>

// Transceptor LoRa en la placa Heltec WiFi LoRa32 V3:
// NSS pin:         SS = 8
// DIO0 pin (irq):  DIO0 = 14
// RESET pin:       RST_LoRa = 12
// DIO1 pin:        BUSY_LoRa = 13
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);

void setup()
{
  Serial.begin(115200);
  delay(10000); // Damos tiempo a abrir la terminal serie

  // Inicializar SX1262 con los parametros por defecto
  Serial.print("Inicializando... ");
  // Freq:              868.0
  // Bandwidth:         125
  // Spreading factor:   7
  int state = radio.begin(868.0, 125, 7);

  // Reducimos potencia a 0dBm (1mW) pruebas de corta distancia
  radio.setOutputPower(0);

  // Gestión de errores
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println("OK!");
  } 
  else
  {
    Serial.print("Error, codigo: ");
    Serial.println(state);
    while (true);
  }
}

void loop()
{
  Serial.print("Transmitiendo paquete... ");
  String datos = "ESCRIBE AQUI TU NOMBRE";
  int state = radio.transmit(datos);

  // Gestión de errores
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(" ENVIADO!");
  }
  else
  {
    Serial.print("Error, codigo: ");
    Serial.println(state);
  }

  // Esperamos 15 segundos y transmitimos de nuevo
  delay(15000);
}