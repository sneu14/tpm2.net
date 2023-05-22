#include <Arduino.h>
#include <ESP8266WiFi.h>

// needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <WiFiUdp.h>

#define LEDNUM 300

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
  UDP.beginPacket(WLEDIP, 65506);
  UDP.write(0x9c);
  UDP.write(0xda);
  UDP.write((LEDNUM * 3) >> 8);
  UDP.write((LEDNUM * 3) & 0xff);
  UDP.write(0x01);
  UDP.write(0x01);
  for (int i = 0; i < (LEDNUM * 3); i++)
  {
    UDP.write(ledstream[i]);
  }
  UDP.write(0x36);
  UDP.endPacket();
}