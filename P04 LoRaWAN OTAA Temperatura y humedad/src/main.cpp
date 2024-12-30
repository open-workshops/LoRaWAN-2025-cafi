/***************************************************************************
  P02: Práctica de LoRaWAN Hola Mundo
  
  Este ejemplo permite enviar uplinks
  - Cada 15 segundos se envía un uplink LoRaWAN con un hola mundo

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

    # PAYLOAD FORMATER para TTN #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        myTestValue: String.fromCharCode.apply(null, bytes)
      };
    }

 ****************************************************************************/
#include <Arduino.h>
#include <RadioLib.h> 
#include "SHT85.h"

const LoRaWANBand_t Region = EU868;

uint64_t joinEUI =   0x00000000;
uint64_t devEUI  =   0x00000000;
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t nwkKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Transceptor LoRa en la placa Heltec WiFi LoRa32 V3 3.1:
// NSS pin:         SS = 8
// DIO0 pin (irq):  DIO0 = 14
// RESET pin:       RST_LoRa = 12
// DIO1 pin:        BUSY_LoRa = 13
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
LoRaWANNode node(&radio, &Region);

SHT30 sht(0x44);

void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando");

  radio.begin();

  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  node.activateOTAA();

  // Inicialización del sensor SHT30
  Wire.begin(SDA, SCL, 100000U);
  sht.begin();
}

void loop()
{
  if (!sht.read())  // solicitamos medición de temperatura y humedad al sensor
  {
    // si ha habido un error, no continuamos
    Serial.print("Error leyendo el sensor: ");
    Serial.println(sht.getError());
    delay(10000);
  }
  
  // Texto: 22.56C 56.21%
  // String payloadSrt = String(sht.getTemperature(), 2) + "C " + String(sht.getHumidity(), 2) + "%";
  // Serial.print(payloadSrt);
  // uint8_t *payload = (uint8_t *) payloadSrt.c_str();
  // int16_t state = node.sendReceive(payload, 13);

  // Numeros enteros: [22,56]
  // uint8_t payload[2];
  // payload[0] = sht.getTemperature();
  // payload[1] = sht.getHumidity();
  // Serial.print(payload[0]); 
  // Serial.print(" "); 
  // Serial.print(payload[1]);
  // int16_t state = node.sendReceive(payload, 2);

  // Payload escalado
  uint8_t payload[2];
  payload[0] = (uint8_t) ((sht.getTemperature() + 12) * 5);
  payload[1] = (uint8_t) sht.getHumidity();
  Serial.print(payload[0]); 
  Serial.print(" "); 
  Serial.print(payload[1]);

  uint8_t downlinkPayload[10];  
  size_t  downlinkSize; 
  int16_t state = node.sendReceive(payload, 2, 1, downlinkPayload, &downlinkSize);

  if (state > 0) // state = 1 downlink en RX1, state = 2 downlink en RX2
  {
    Serial.println("-- NUEVO DOWNLINK");
    Serial.println("Recibidos " + String(downlinkSize) + " bytes");
    Serial.println(payload[0]);
  }
  
  Serial.println(" Enviando uplink...");
  delay(15000);
}