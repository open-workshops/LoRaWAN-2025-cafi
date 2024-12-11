#include <HardwareSerial.h>
#include "TinyGPS++.h" //https://github.com/mikalhart/TinyGPSPlus
#include "gps.h"
#include "oled.h"

TinyGPSPlus gps; // Objeto TinyGPS++
HardwareSerial gpsSerial(1); // Conexión Serie con GPS

// Versión custom de delay() para no perder comunicaciones con el GPS
void smartGPSdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

void setupGPS(void)
{
  gpsSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);
}

void printGPSdata(void)
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "GPS");
  display.setFont(ArialMT_Plain_10);
 
  char text[32];
  sprintf(text, "%02d/%02d/%02d ", gps.date.day(), gps.date.month(), gps.date.year());
  display.drawString(64,0, text);

  sprintf(text, "%02d:%02d:%02d UTC", gps.time.hour(), gps.time.minute(), gps.time.second());
  display.drawString(64,10, text);

  if(gps.location.isValid())
  {
    sprintf(text, "Lat:    %.6f", gps.location.lat());
    display.drawString(0,22, text);

    sprintf(text, "Long:  %.6f", gps.location.lng());
    display.drawString(0,32, text);
  }

  if(gps.altitude.isValid())
  {
    sprintf(text, "Alt:     %.1fm", gps.altitude.meters());
    display.drawString(0,42, text);
  }

  if(gps.speed.isValid())
  {
    sprintf(text, "Speed: %.0f Km/h", gps.speed.kmph());
    display.drawString(0,52, text);
  }

  sprintf(text, "Sats:  %i", gps.satellites.value());
  display.drawString(82,42, text);

  sprintf(text, "Hdop:%.1f", gps.hdop.hdop());
  display.drawString(82,52, text); 

  display.display();
}

void buildGPSpacket(uint8_t txBuffer[9])
{
  uint32_t LatitudeBinary, LongitudeBinary;
  uint16_t altitudeGps;
  uint8_t hdopGps;
  LatitudeBinary = ((gps.location.lat() + 90) / 180.0) * 16777215;
  LongitudeBinary = ((gps.location.lng() + 180) / 360.0) * 16777215;
  
  txBuffer[0] = ( LatitudeBinary >> 16 ) & 0xFF;
  txBuffer[1] = ( LatitudeBinary >> 8 ) & 0xFF;
  txBuffer[2] = LatitudeBinary & 0xFF;

  txBuffer[3] = ( LongitudeBinary >> 16 ) & 0xFF;
  txBuffer[4] = ( LongitudeBinary >> 8 ) & 0xFF;
  txBuffer[5] = LongitudeBinary & 0xFF;

  altitudeGps = gps.altitude.meters();
  txBuffer[6] = ( altitudeGps >> 8 ) & 0xFF;
  txBuffer[7] = altitudeGps & 0xFF;

  hdopGps = gps.hdop.hdop()/10;
  txBuffer[8] = hdopGps & 0xFF;
}

bool checkGPSFix(void)
{
  if (gps.location.isValid() && 
      gps.location.age() < 2000 &&
      gps.hdop.isValid() &&
      gps.hdop.hdop() <= 300 &&
      gps.hdop.age() < 2000 &&
      gps.altitude.isValid() && 
      gps.altitude.age() < 2000 )
  {
    Serial.println("Valid GPS Fix.");
    /*
    display.setFont(ArialMT_Plain_10);
    display.setColor(BLACK);display.fillRect(0, 21, 54, 10);display.display();display.setColor(WHITE);
    display.drawString(0, 17, "GPS: FIX");
    display.display();*/
    return true;
  }
  else
  {
     Serial.println("No GPS Fix.");
     /*
     display.setFont(ArialMT_Plain_10);
     display.setColor(BLACK);display.fillRect(0, 21, 54, 10);display.display();display.setColor(WHITE);
     display.drawString(0, 17, "GPS: NO");
     display.display();

     sprintf(t, "location valid: %i" , tGps.location.isValid());
     Serial.println(t);
     sprintf(t, "location age: %i" , tGps.location.age());
     Serial.println(t);
     sprintf(t, "hdop valid: %i" , tGps.hdop.isValid());
     Serial.println(t);
     sprintf(t, "hdop age: %i" , tGps.hdop.age());
     Serial.println(t);
     sprintf(t, "hdop: %i" , tGps.hdop.value());
     Serial.println(t);
     sprintf(t, "altitude valid: %i" , tGps.altitude.isValid());
     Serial.println(t);
     sprintf(t, "altitude age: %i" , tGps.altitude.age());
     Serial.println(t);
     */

    return false;
  }
}