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

const LoRaWANBand_t Region = EU868;

uint32_t devAddr =        0x00000000;
uint8_t fNwkSIntKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t sNwkSIntKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t nwkSEncKey[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] =     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Transceptor LoRa en la placa Heltec WiFi LoRa32 V3 3.1:
// NSS pin:         SS = 8
// DIO0 pin (irq):  DIO0 = 14
// RESET pin:       RST_LoRa = 12
// DIO1 pin:        BUSY_LoRa = 13
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
LoRaWANNode node(&radio, &Region);

void setup()
{
  Serial.begin(115200);
  delay(20000);
  Serial.println("Iniciando LoRaWAN ABP");

  radio.begin();

  node.beginABP(devAddr, fNwkSIntKey, sNwkSIntKey, nwkSEncKey, appSKey);

  node.activateABP();
}

void loop()
{
  uint8_t *payload = (uint8_t *)"hola ABP";

  Serial.println("Enviando uplink.");

  node.sendReceive(payload, 8);
  
  delay(15000);
}





































/*
//nonces
#include <Preferences.h>
Preferences store;


void restoreNounce()
{
  store.begin("radiolib");
  if (store.isKey("nonces")) 
  {
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
    store.getBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
    node.setBufferNonces(buffer);
    uint16_t devNonces = ((uint16_t)buffer[9])<<8 | buffer[8];
    uint32_t joinNonces = ((uint32_t)buffer[12])<<16 | ((uint32_t)buffer[11])<<8 | buffer[10];
    Serial.println("devNonces:" + String(devNonces) + " joinNonces:"+ String(joinNonces));
  }            
}

void saveNounce()
{
  uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
  uint8_t *persist = node.getBufferNonces();
  memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
  store.putBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
}

*/