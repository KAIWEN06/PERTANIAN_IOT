#include "painlessMesh.h"

#define MESH_PREFIX "BANANA_MESH"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg)
{
    Serial.print("Dari Node ");
    Serial.print(from);
    Serial.print(" : ");
    Serial.println(msg);
}

void setup()
{
    Serial.begin(115200);

    mesh.init(
        MESH_PREFIX,
        MESH_PASSWORD,
        &userScheduler,
        MESH_PORT);

    mesh.onReceive(receivedCallback);

    Serial.println("Gateway Aktif");
}

void loop()
{
    mesh.update();
}