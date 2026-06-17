#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "BANANA_SYNC";
const char *password = "12345678";

WiFiUDP udp;

#define LED_PIN 2

unsigned long lastSend = 0;

unsigned long masterTime = 0;
unsigned long localSyncMillis = 0;

bool connected = false;

unsigned long getVirtualTime()
{
  return masterTime +
         (millis() - localSyncMillis);
}

void setup()
{
  Serial.begin(9600);

  pinMode(
      LED_PIN,
      OUTPUT);

  WiFi.begin(
      ssid,
      password);

  Serial.print("Connecting");

  while (
      WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  connected = true;

  Serial.println();
  Serial.println("CONNECTED");

  Serial.print("IP : ");
  Serial.println(
      WiFi.localIP());
}

void loop()
{
  int packetSize = udp.parsePacket();

  if (packetSize)
  {
    char packet[100];

    int len =
        udp.read(
            packet,
            sizeof(packet) - 1);

    if (len > 0)
    {
      packet[len] = '\0';
    }

    String msg(packet);

    if (msg.startsWith("SYNC:"))
    {
      masterTime =
          msg.substring(5).toInt();

      localSyncMillis =
          millis();
    }
  }

  if (
      millis() - lastSend >= 1000)
  {
    lastSend = millis();

    int rssi = WiFi.RSSI();

    String msg =
        "POHON 1"
        "|RSSI=" +
        String(rssi) +
        "|TIME=" +
        String(
            getVirtualTime());

    udp.beginPacket(
        "192.168.4.1",
        4210);

    udp.print(msg);

    udp.endPacket();

    Serial.print(
        "SEND : ");

    Serial.println(msg);
  }

  unsigned long phase =
      getVirtualTime() % 1000;

  if (connected)
  {
    if (
        (phase < 100) ||
        (phase >= 200 &&
         phase < 300))
    {
      digitalWrite(
          LED_PIN,
          HIGH);
    }
    else
    {
      digitalWrite(
          LED_PIN,
          LOW);
    }
  }
  else
  {
    if (phase < 100)
    {
      digitalWrite(
          LED_PIN,
          HIGH);
    }
    else
    {
      digitalWrite(
          LED_PIN,
          LOW);
    }
  }
}