/***************************************************************************
  P04: Práctica de LoRaWAN y Sensor de temperatura, humedad y CO2
  
  Este ejemplo permite enviar datos de un sensor con activacion por OTAA
  - Inicalmente se hace un JOIN por OTAA a una red LoRaWAN
  - La placa lee el estado de un sensor SHT30 (temperatura y humedad)
  - La placa lee el estado de un sensor MHZ19C (CO2)
  - Los valores de temperatura, humedad y CO2 se imprimen por serial
  - Se formatean los valores anteriores en un array para enviarlos por red
  - Se realiza el uplink por LoRaWAN

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes.
  
  El siguiente payload formatter permite decodificar el payload adecuadamente

    # PAYLOAD FORMATER para PAYLOAD ESCALADO #

    function Decoder(bytes, port) {
      return {
        temperature: bytes[0] / 5.0 - 12.0,
        humidity: bytes[1],
        co2: bytes[2] * 18 + 400,
        puerta: bytes[i] ? "Bloqueada" : "Desbloqueada"
      };
    }

 ****************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>
#include <Preferences.h>
#include "SHT85.h"
#include "MHZ19.h"

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

#define RX_PIN 33           // Rx del ESP32 al que está conectado el TX del MHZ19
#define TX_PIN 34           // Tx del ESP32 al que está conectado el RX del MHZ19
#define BAUDRATE_MHZ19 9600 // Serial baudrate del MH-Z19 (no debe modificarse)

#define PUERTA_PIN1 19 // Pin 1 del sensor de puerta
#define PUERTA_PIN2 20 // Pin 2 del sensor de puerta

MHZ19 myMHZ19;
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

  Serial2.begin(BAUDRATE_MHZ19, SERIAL_8N1, RX_PIN, TX_PIN); // Inicializamos un nuvo puerto serie para el MHZ19
  myMHZ19.begin(Serial2);                                    // Le indicamos a la librería que puerto usar
  myMHZ19.autoCalibration();                                 // Turn auto calibration ON (OFF autoCalibration(false))

  // Deberiamos esperar un minuto a que el sensor se caliente antes de leer
  // Serial.println("Precalentando sensor (60s)");
  // delay(60000); // precalentando sensor (60s)

  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(PUERTA_PIN1, INPUT_PULLUP); // pin de lectura del sensor de puerta
  pinMode(PUERTA_PIN2, OUTPUT); 
  digitalWrite(PUERTA_PIN2, LOW); // ponemos el pin2 a LOW para simular GND
}

void loop()
{
  int puertaAbierta = digitalRead(PUERTA_PIN1);

  if (!sht.read())  // solicitamos medición de temperatura y humedad al sensor
  {
    // si ha habido un error, no continuamos
    Serial.print("Error leyendo el sensor: ");
    Serial.println(sht.getError());
    delay(10000);
  }
  
  uint8_t downlinkPayload[10];  
  size_t  downlinkSize;

  // Payload escalado
  uint8_t payload[4];
  payload[0] = (uint8_t) ((sht.getTemperature() + 12) * 5);
  payload[1] = (uint8_t) sht.getHumidity();
  payload[2] = (uint8_t) ((myMHZ19.getCO2() - 400) / 18);
  payload[3] = puertaAbierta;
  Serial.print("Payload: ");
  Serial.print(payload[0]); 
  Serial.print(" "); 
  Serial.print(payload[1]);
  Serial.print(" "); 
  Serial.print(payload[2]);
  Serial.print(" "); 
  Serial.print(payload[3]);
  Serial.println(" Enviando uplink.");
  int16_t state = node.sendReceive(payload, 4, 1, downlinkPayload, &downlinkSize);

  if (state > 0) // state = 1 downlink en RX1, state = 2 downlink en RX2
  {
    Serial.print(">Recibido DOWNLINK con " + String(downlinkSize) + " bytes en RX" + String(state) + ":");
    for(int i = 0; i<downlinkSize; i++)
    {
      Serial.print(" ");
      Serial.print(downlinkPayload[i], HEX);
    } 
    Serial.println();

    if (downlinkSize == 1)
    {
      if (downlinkPayload[0] == 1)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("Puerta bloqueada");
      }
      else
      {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("Puerta desbloqueada");
      }
    }
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
