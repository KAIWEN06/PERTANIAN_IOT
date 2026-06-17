#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555
#define node_name "gateway"

Scheduler userScheduler;
painlessMesh mesh;

unsigned long lastSend = 0;

void setup()
{
    Serial.begin(9600);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    randomSeed(analogRead(0));

    Serial.println("Node Aktif");
}

void loop()
{
    mesh.update();

    if (millis() - lastSend > 2000)
    {
        lastSend = millis();

        int angka = random(0, 1000);

        String pesan =
            "NODE_ID=" +
            String(mesh.getNodeId()) + "NODE_NAME" + node_name +
            ",DATA=" +
            String(angka);

        mesh.sendBroadcast(pesan);

        Serial.println("Kirim: " + pesan);
    }
}