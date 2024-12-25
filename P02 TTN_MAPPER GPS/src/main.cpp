/*******************************************************************************
  P03: Práctica TTN MAPPER GPS
  
  Este ejemplo mapea la cobertura de la red TTN utilizando LoRaWAN y GPS.
  -La placa está siempre leyendo el estado del GPS.
  -En la pantalla se proporciona información en cuanto está disponible.
  -Mientras el GPS no obtiene fix mantiene su led encendido.
  -Cuando el GPS recibe fix correcto, hace parpadear su led.
  -Utiliza tipo de sesión ABP y SF7

  Conexiones:
  GPS       Placa Heltec LoRa32
  VCC       Pin 3V3
  GND       Pin GND
  TX        PIN 45 (RX)
  RX        PIN 46 (RX)
  PPS       No conectar

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

  # PAYLOAD FORMATER para TTN #

  function Decoder(bytes, port) {
    var decoded = {};

    decoded.latitude = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
    decoded.latitude = (decoded.latitude / 16777215.0 * 180) - 90;
  
    decoded.longitude = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
    decoded.longitude = (decoded.longitude / 16777215.0 * 360) - 180;
  
    var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
    var sign = bytes[6] & (1 << 7);
    if(sign)
    {
        decoded.altitude = 0xFFFF0000 | altValue;
    }
    else
    {
        decoded.altitude = altValue;
    }
  
    decoded.hdop = bytes[8] / 10.0;

    return decoded;
  }

*******************************************************************************/
#include <Arduino.h>
#include <RadioLib.h> 
#include "gps.h"
#include "oled.h"

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
uint8_t txBuffer[9];

void setup()
{
  Serial.begin(115200);
  delay(10000); // Damos tiempo a abrir la terminal serie
  Serial.println("Setup OLED");
  setupOLED();
  Serial.println("Setup GPS");
  setupGPS();
  Serial.println("Inicializando TTN Mapper...");

  // Configuración chip LoRa
  if (radio.begin() == RADIOLIB_ERR_NONE)
    Serial.println("Chip LoRa configurado OK");
  else
  {
    Serial.println("ERROR configurando chip LoRa!");
    while(1) {delay(1);}
  }

  // Provisionado de claves ABP
  if (node.beginABP(devAddr, fNwkSIntKey, sNwkSIntKey, nwkSEncKey, appSKey) 
        == RADIOLIB_ERR_NONE )
    Serial.println("Provisionado de claves OK");
  else
  {
    Serial.println("ERROR provisionando claves!");
    while(1) {delay(1);}
  }

  // Activación ABP
  node.activateABP();
  Serial.println("Activación ABP OK");
}

void loop()
{
  if (checkGPSFix()) // Comprueba que tenemos GPS fix
  {
    buildGPSpacket(txBuffer); // rellenado payload

    // Envío paquete LoRaWAN
    Serial.print("Enviando paquete LoRaWAN...");
    int state = node.sendReceive(txBuffer, sizeof(txBuffer));    

    // Gestión de errores
    if (state < RADIOLIB_ERR_NONE)
      Serial.println("Error en sendReceive");
    else
      Serial.println("Paquete enviado.");
    
    if(state > 0) 
      Serial.println("Recibido downlink\n");
    else
      Serial.println();
  }
  printGPSdata(); // Mostramos datos GPS en pantalla
  smartGPSdelay(30000);
}