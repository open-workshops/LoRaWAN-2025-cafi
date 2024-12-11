/*******************************************************************************
  P1: Práctica de LoRa P2P (Transmisor)
  
  Este ejemplo permite enviar tramas de LoRa a bajo nivel.
 ******************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>

// Transceptor LoRa conectado en la placa Heltec WiFi LoRa32 V3 en los pines:
// NSS pin:   SS = 8
// DIO0 pin (irq):  DIO0 = 14
// RESET pin: RST_LoRa = 12
// DIO1 pin:  DIO1 = 13
SX1276 radio = new Module(SS, DIO0, RST_LoRa, 13);

void setup()
{
  Serial.begin(115200);
  delay(4000); // esperamos a que el minitor serial este listo

  // inicializar SX1276 con los parametros por defecto
  Serial.print("Inicializando... ");

  // Freq: 868.0
  // Bandwidth: 125
  // Spreading factor: 7
  int state = radio.begin(868.0, 125, 7);

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


  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(" ENVIADO!");
  }
  else
  {
    Serial.print("Error, codigo: ");
    Serial.println(state);
  }

  // esperamos 5 segundos y transmitimos de nuevo
  delay(15000);
}