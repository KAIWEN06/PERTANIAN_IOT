#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h>

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

#define NODE_NAME "NODE_3"

// LED bawaan NodeMCU (aktif LOW)
#define LED_PIN LED_BUILTIN

Scheduler userScheduler;
painlessMesh mesh;

bool ackReceived = false;

unsigned long lastSend = 0;
unsigned long lastAck = 0;
unsigned long ledUntil = 0;
unsigned long lastDebug = 0;

// =====================================================
// CALLBACKS
// =====================================================

void receivedCallback(uint32_t from, String &msg)
{
    Serial.print("FROM ");
    Serial.print(from);
    Serial.print(" : ");
    Serial.println(msg);

    if (msg == "ACK")
    {
        ackReceived = true;
        lastAck = millis();

        ledUntil = millis() + 300;

        Serial.println("ACK RECEIVED");
    }
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.println();
    Serial.println("================================");
    Serial.println("NEW CONNECTION");
    Serial.println("================================");

    Serial.print("Node : ");
    Serial.println(nodeId);
}

void changedConnectionCallback()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("TOPOLOGY CHANGED");
    Serial.println("================================");

    Serial.println(mesh.subConnectionJson());
}

void droppedConnectionCallback(uint32_t nodeId)
{
    Serial.println();
    Serial.println("================================");
    Serial.println("LOST CONNECTION");
    Serial.println("================================");

    Serial.print("Node : ");
    Serial.println(nodeId);
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    pinMode(LED_PIN, OUTPUT);

    // LED OFF
    digitalWrite(LED_PIN, HIGH);

    WiFi.mode(WIFI_STA);

    // Matikan WiFi Sleep ESP8266
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    // Cari root gateway
    mesh.setContainsRoot(true);

    mesh.onReceive(
        receivedCallback);

    mesh.onNewConnection(
        newConnectionCallback);

    mesh.onChangedConnections(
        changedConnectionCallback);

    mesh.onDroppedConnection(
        droppedConnectionCallback);

    randomSeed(micros());

    Serial.println();
    Serial.println("================================");
    Serial.println("NODEMCU START");
    Serial.println("================================");

    Serial.print("Mesh ID : ");
    Serial.println(mesh.getNodeId());
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    mesh.update();

    if (millis() - lastSend > 2000)
    {
        lastSend = millis();

        ackReceived = false;

        int dataSensor = random(0, 1000);

        StaticJsonDocument<256> doc;

        doc["nodeId"] = mesh.getNodeId();
        doc["nodeName"] = NODE_NAME;
        doc["sensor"] = dataSensor;
        doc["status"] = "ONLINE";

        String msg;
        serializeJson(doc, msg);

        Serial.println("SEND DATA");
        Serial.println(msg);

        mesh.sendBroadcast(msg);
    }

    // LED aktif LOW
    if (ledUntil > millis())
    {
        digitalWrite(LED_PIN, LOW);
    }
    else
    {
        digitalWrite(LED_PIN, HIGH);
    }

    if (millis() - lastDebug > 10000)
    {
        lastDebug = millis();

        Serial.println();
        Serial.println("========== DEBUG ==========");

        Serial.print("Free Heap : ");
        Serial.println(ESP.getFreeHeap());

        Serial.print("RSSI : ");
        Serial.println(WiFi.RSSI());

        Serial.print("ACK Age : ");
        Serial.println(millis() - lastAck);

        Serial.println("===========================");
    }
}