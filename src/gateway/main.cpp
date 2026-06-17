// GATEWAY NODE

#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

#define NODE_NAME "GATEWAY"

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg)
{
    Serial.println("================================");
    Serial.print("Dari Mesh ID : ");
    Serial.println(from);

    Serial.print("Pesan        : ");
    Serial.println(msg);
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.print("Node Join : ");
    Serial.println(nodeId);
}

void setup()
{
    Serial.begin(9600);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    mesh.onReceive(receivedCallback);
    mesh.onNewConnection(newConnectionCallback);

    Serial.println("Gateway Aktif");
    Serial.print("Mesh ID Gateway : ");
    Serial.println(mesh.getNodeId());
}

void loop()
{
    mesh.update();
}