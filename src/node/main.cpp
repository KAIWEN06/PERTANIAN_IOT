#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

#define NODE_NAME "NODE_1"
#define LED_PIN 2

Scheduler userScheduler;
painlessMesh mesh;
bool ackReceived = false;

unsigned long lastSend = 0;

void receivedCallback(uint32_t from, String &msg)
{
    Serial.print("Dari ");
    Serial.print(from);
    Serial.print(" : ");
    Serial.println(msg);

    if (msg == "ACK")
    {
        ackReceived = true;
        digitalWrite(LED_PIN, HIGH);

        Serial.println("DATA BERHASIL DITERIMA GATEWAY");
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
        Serial.println("DATA GAGAL DITERIMA GATEWAY");
    }

    void setup()
    {
        Serial.begin(9600);

        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LOW);

        WiFi.mode(WIFI_STA);

        WiFi.setTxPower(
            WIFI_POWER_19_5dBm);

        mesh.init(
            MESH_PREFIX,
            MESH_PASSWORD,
            &userScheduler,
            MESH_PORT);

        mesh.onReceive(
            receivedCallback);

        randomSeed(
            analogRead(0));

        Serial.println();
        Serial.println("NODE AKTIF");

        Serial.print("Mesh ID : ");
        Serial.println(
            mesh.getNodeId());
    }

    void loop()
    {
        mesh.update();

        if (
            millis() - lastSend > 2000)
        {
            lastSend = millis();

            int dataSensor =
                random(0, 1000);

            int rssi =
                WiFi.RSSI();

            String pesan =
                "NODE_ID=" +
                String(mesh.getNodeId()) +

                ",NODE_NAME=" +
                String(NODE_NAME) +

                ",RSSI=" +
                String(rssi) +
                "dBm" +

                ",DATA=" +
                String(dataSensor);

            mesh.sendBroadcast(
                pesan);

            Serial.println(
                "Kirim : " +
                pesan);
        }
    }