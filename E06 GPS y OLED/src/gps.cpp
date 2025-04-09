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

void updateGPS(void)
{
    while (gpsSerial.available() > 0) 
    {
        gps.encode(gpsSerial.read());
    }
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
