#include <Arduino.h>
#include <ESP8266WiFi.h>

// needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <WiFiUdp.h>

#define LEDNUM 300
#define PSPLIT 125

void sendTpm2packets();

WiFiUDP UDP;

byte ledstream[LEDNUM * 3];

IPAddress WLEDIP(192, 168, 5, 169);

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.resetSettings();

  wifiManager.autoConnect("OLR-AP");

  Serial.println("WLAN connected...yeey :)");
  for (int i = 0; i < LEDNUM * 3; i++)
  {
    ledstream[i] = 0;
  }
}

void loop()
{
  ledstream[0] = 0xff;
  ledstream[LEDNUM * 3 - 1] = 0x00;
  sendTpm2packets();
  delay(1000);
  ledstream[0] = 0x00;
  ledstream[LEDNUM * 3 - 1] = 0xff;
  sendTpm2packets();
  delay(1000);
}

void sendTpm2packets()
{
  // Anzahl der vollständigen Pakete bestimmen
  uint8_t packetcount = LEDNUM / PSPLIT;

  // Wenn es noch einen Rest gibt der kleiner ist als PSPLIT
  if (LEDNUM % PSPLIT > 0)
  {
    packetcount++;
  }
  // Groesse des letzten Paketes berechnen
  uint lastpacketcount = (LEDNUM % PSPLIT) * 3;

  // den Zähler für die Anzahl der volständigen Pakete berechnen
  uint8_t packetloop = packetcount;
  if (lastpacketcount > 0)
  {
    packetloop--;
  }

  for (int j = 0; j < packetloop; j++)
  {
    UDP.beginPacket(WLEDIP, 65506);
    UDP.write(0x9c);
    UDP.write(0xda);
    UDP.write((PSPLIT * 3) >> 8);
    UDP.write((PSPLIT * 3) & 0xff);

    UDP.write(j + 1);
    UDP.write(packetcount);
    for (int i = PSPLIT * 3 * j; i < PSPLIT * 3 * (j + 1); i++)
    {
      UDP.write(ledstream[i]);
    }
    UDP.write(0x36);
    UDP.endPacket();
  }
  if (lastpacketcount > 0)
  {
    UDP.beginPacket(WLEDIP, 65506);
    UDP.write(0x9c);
    UDP.write(0xda);
    UDP.write(lastpacketcount >> 8);
    UDP.write(lastpacketcount & 0xff);

    UDP.write(packetcount);
    UDP.write(packetcount);
    for (int i = LEDNUM * 3 - lastpacketcount; i < LEDNUM * 3; i++)
    {
      UDP.write(ledstream[i]);
    }
    UDP.write(0x36);
    UDP.endPacket();
  }
}