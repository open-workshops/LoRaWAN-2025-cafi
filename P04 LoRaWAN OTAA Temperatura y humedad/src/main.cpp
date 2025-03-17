/***************************************************************************
  P04: Práctica de LoRaWAN y Sensor de temperatura y humedad
  
  Este ejemplo permite enviar datos de un sensor con activacion por OTAA
  - Inicalmente se hace un JOIN por OTAA a una red LoRaWAN
  - La placa lee el estado de un sensor SHT30 (temperatura y humedad)
  - Los valores de temperatura y humedad se imprimen por serial
  - Se formatean los valores anteriores en un array para enviarlos por red
  - Se realiza el uplink por LoRaWAN

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes.
  
  Los siguientes payload formatters se corresponden con las diferentes
  formas de enviar los payloads comentadas en la parte final del loop

    # PAYLOAD FORMATER para TEXTO #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        myTestValue: String.fromCharCode.apply(null, bytes)
      };
    }


    # PAYLOAD FORMATER para NUMEROS ENTEROS #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        temperature: bytes[0],
        humidity: bytes[1]
      };
    }


    # PAYLOAD FORMATER para PAYLOAD ESCALADO #

    function Decoder(bytes, port) {
      return {
        temperature: bytes[0] / 5.0 - 12.0,
        humidity: bytes[1]
      };
    }

 ****************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>
#include <Preferences.h> 
#include "SHT85.h"

const LoRaWANBand_t Region = EU868;
Preferences store;

void restoreNounce();
void saveNounce();

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
  delay(20000);
  Serial.println("Iniciando LoRaWAN OTAA");

  radio.begin();

  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  Serial.print("\nOTA Join...");
  restoreNounce();
  int16_t state = node.activateOTAA();
  
  if(state == RADIOLIB_LORAWAN_NEW_SESSION)
  {
    Serial.println("Join OK!");
    saveNounce();
  }
  else
  {
    Serial.println("Join FAIL:( reiniciando\n");
    saveNounce();
    ESP.restart();
  }

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
  
  uint8_t downlinkPayload[10];  
  size_t  downlinkSize;

  // Texto: 22.56C 56.21%
  // String payloadSrt = String(sht.getTemperature(), 2) + "C " + String(sht.getHumidity(), 2) + "%";
  // Serial.print("Payload: ");
  // Serial.print(payloadSrt);
  // uint8_t *payload = (uint8_t *) payloadSrt.c_str();
  // Serial.println(" Enviando uplink.");
  // int16_t state = node.sendReceive(payload, 13, 1, downlinkPayload, &downlinkSize);

  // Numeros enteros: [22,56]
  // uint8_t payload[2];
  // payload[0] = sht.getTemperature();
  // payload[1] = sht.getHumidity();
  // Serial.print("Payload: ");
  // Serial.print(payload[0]); 
  // Serial.print(" "); 
  // Serial.print(payload[1]);
  // Serial.println(" Enviando uplink.");
  // int16_t state = node.sendReceive(payload, 2, 1, downlinkPayload, &downlinkSize);

  // Payload escalado
  uint8_t payload[2];
  payload[0] = (uint8_t) ((sht.getTemperature() + 12) * 5);
  payload[1] = (uint8_t) sht.getHumidity();
  Serial.print("Payload: ");
  Serial.print(payload[0]); 
  Serial.print(" "); 
  Serial.print(payload[1]);
  Serial.println(" Enviando uplink.");
  int16_t state = node.sendReceive(payload, 2, 1, downlinkPayload, &downlinkSize);

  if (state > 0) // state = 1 downlink en RX1, state = 2 downlink en RX2
  {
    Serial.print(">Recibido DOWNLINK con " + String(downlinkSize) + " bytes en RX" + String(state));
    for(int i = 0; i<downlinkSize; i++)
    {
      Serial.print(" ");
      Serial.print(downlinkPayload[i],HEX);
    } 
    Serial.println();
  }  
  delay(15000);
}

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
