#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

#define NODE_NAME "GATEWAY"

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg)
{
    Serial.println();
    Serial.println("================================");

    Serial.print("Mesh ID Node : ");
    Serial.println(from);

    Serial.print("Pesan        : ");
    Serial.println(msg);

    Serial.println("================================");
    mesh.sendSingle(
        from,
        "ACK");
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.println();

    Serial.print("Node Join : ");
    Serial.println(nodeId);

    Serial.print("Jumlah Node : ");
    Serial.println(mesh.getNodeList().size());
}

void changedConnectionCallback()
{
    Serial.println("Topologi Mesh Berubah");
}

void setup()
{
    Serial.begin(9600);

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

    mesh.onNewConnection(
        newConnectionCallback);

    mesh.onChangedConnections(
        changedConnectionCallback);

    Serial.println();
    Serial.println("GATEWAY AKTIF");

    Serial.print("Mesh ID Gateway : ");
    Serial.println(
        mesh.getNodeId());
}

void loop()
{
    mesh.update();
}