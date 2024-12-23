/*******************************************************************************
  P1: Práctica de LoRa P2P (Receptor)
  
  Este ejemplo permite recibir las tramas de LoRa enviadas por el transmisor.
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
  Serial.print("Escuchando a la espera de una transmisión... ");
}

void loop() {
  String str;
  int state = radio.receive(str);

  // Gestión de errores
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println("Recibido!");

    Serial.print("Data:\t");
    Serial.println(str);

    Serial.print("RSSI:\t");
    Serial.print(radio.getRSSI());
    Serial.println(" dBm");

    Serial.print("SNR:\t");
    Serial.print(radio.getSNR());
    Serial.println(" dB");

    Serial.print("Escuchando a la espera de una transmisión ... "); 
  } 
  else if (state != RADIOLIB_ERR_RX_TIMEOUT)
  {
    Serial.print("Error, codigo: ");
    Serial.println(state);
  }
}
