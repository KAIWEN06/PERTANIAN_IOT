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
unsigned long lastAck = 0;

void receivedCallback(uint32_t from, String &msg)
{
    Serial.print("Dari ");
    Serial.print(from);
    Serial.print(" : ");
    Serial.println(msg);

    if (msg == "ACK")
    {
        ackReceived = true;
        lastAck = millis();

        digitalWrite(LED_PIN, HIGH);

        Serial.println("DATA BERHASIL DITERIMA GATEWAY");
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    mesh.onReceive(receivedCallback);

    randomSeed(millis());

    Serial.println();
    Serial.println("NODE AKTIF");

    Serial.print("Mesh ID : ");
    Serial.println(mesh.getNodeId());
}

void loop()
{
    mesh.update();

    if (millis() - lastSend > 2000)
    {
        lastSend = millis();

        ackReceived = false;

        // Saat kirim, anggap belum berhasil
        digitalWrite(LED_PIN, LOW);

        int dataSensor = random(0, 1000);

        String pesan =
            "NODE_ID=" +
            String(mesh.getNodeId()) +

            ",NODE_NAME=" +
            String(NODE_NAME) +

            ",DATA=" +
            String(dataSensor);

        mesh.sendBroadcast(pesan);

        Serial.println("Kirim : " + pesan);
    }

    // Jika 5 detik tidak ada ACK
    if (!ackReceived &&
        millis() - lastSend > 2000)
    {
        digitalWrite(LED_PIN, LOW);
    }
}