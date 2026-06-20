#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

unsigned long lastTopologySend = 0;
unsigned long lastDebug = 0;

// =====================================================
// HELPER
// =====================================================

void sendTopology()
{
    String topo = mesh.subConnectionJson();

    Serial.println("TOPOLOGY:" + topo);
}

// =====================================================
// CALLBACKS
// =====================================================

void receivedCallback(uint32_t from, String &msg)
{
    Serial.println("TELEMETRY:" + msg);

    // ACK ke node pengirim
    mesh.sendSingle(from, "ACK");
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.println();
    Serial.println("================================");
    Serial.println("NODE JOIN");
    Serial.println("================================");

    Serial.print("Node ID : ");
    Serial.println(nodeId);

    Serial.print("Total Node : ");
    Serial.println(mesh.getNodeList().size());

    sendTopology();
}

void changedConnectionCallback()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("TOPOLOGY CHANGED");
    Serial.println("================================");

    sendTopology();
}

void droppedConnectionCallback(uint32_t nodeId)
{
    Serial.println();
    Serial.println("================================");
    Serial.println("NODE LOST");
    Serial.println("================================");

    Serial.print("Node ID : ");
    Serial.println(nodeId);

    Serial.print("Sisa Node : ");
    Serial.println(mesh.getNodeList().size());

    sendTopology();
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    WiFi.mode(WIFI_STA);

    // penting untuk mesh
    WiFi.setSleep(false);

    // daya pancar maksimum
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    mesh.setDebugMsgTypes(
        ERROR |
        STARTUP |
        CONNECTION);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    // paksa gateway jadi root
    mesh.setRoot(true);
    mesh.setContainsRoot(true);

    mesh.onReceive(receivedCallback);

    mesh.onNewConnection(
        newConnectionCallback);

    mesh.onChangedConnections(
        changedConnectionCallback);

    mesh.onDroppedConnection(
        droppedConnectionCallback);

    Serial.println();
    Serial.println("================================");
    Serial.println("BANANA MESH GATEWAY");
    Serial.println("================================");

    Serial.print("Gateway ID : ");
    Serial.println(mesh.getNodeId());

    sendTopology();
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    mesh.update();

    // kirim topology setiap 3 detik
    if (millis() - lastTopologySend > 3000)
    {
        lastTopologySend = millis();

        sendTopology();
    }

    // debug setiap 10 detik
    if (millis() - lastDebug > 10000)
    {
        lastDebug = millis();

        Serial.println();
        Serial.println("========== DEBUG ==========");

        Serial.print("Free Heap : ");
        Serial.println(
            ESP.getFreeHeap());

        Serial.print("Node Count : ");
        Serial.println(
            mesh.getNodeList().size());

        Serial.println("===========================");
    }
}