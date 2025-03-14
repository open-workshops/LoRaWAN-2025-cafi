/***************************************************************************
  P03: Práctica de LoRaWAN Hola Mundo OTAA
  
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
#include <Preferences.h>

const LoRaWANBand_t Region = EU868;
Preferences store;

void restoreNounce();
void saveNounce();

uint64_t joinEUI =   0x0000000000000000;
uint64_t devEUI  =   0x0000000000000000;
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t nwkKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Transceptor LoRa en la placa Heltec WiFi LoRa32 V3 3.1:
// NSS pin:         SS = 8
// DIO0 pin (irq):  DIO0 = 14
// RESET pin:       RST_LoRa = 12
// DIO1 pin:        BUSY_LoRa = 13
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
LoRaWANNode node(&radio, &Region);


#define DOOR_SENSOR 6
#define LED_RED 34
#define LED_GREEN 36

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
}

void loop()
{
  int status = digitalRead(DOOR_SENSOR);
  uint8_t downlinkPayload[10];  
  size_t  downlinkSize; 
  uint8_t* payload;

  if (status != lastStatus)
  {
    lastStatus = status;

    if (status == DOOR_CLOSED)
    {
      Serial.println("Enviando puerta CERRADA");
      payload = (uint8_t*) "CERRADA";
    }
    else 
    {
      Serial.println("Enviando puerta ABIERTA");
      payload = (uint8_t*) "ABIERTA";
    }

    int16_t state = node.sendReceive(payload, 7, 1, downlinkPayload, &downlinkSize);

    if (state > 0 && downlinkSize > 0) // state = 1 downlink en RX1, state = 2 downlink en RX2
    {
      Serial.print(">Recibido DOWNLINK con " + String(downlinkSize) + " bytes en RX" + String(state) + ":");
      Serial.println(downlinkPayload[0], HEX);
      
      if (downlinkPayload[0] == 0)
      {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        Serial.println("Puerta desbloqueada");
      }
      else
      {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        Serial.println("Puerta bloqueada");
      }
    }  

    delay(10000);
  }
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
