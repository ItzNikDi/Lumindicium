#include "web_handlers.h"
#include "globals.h"
#include "US100.h"
#include "LittleFS.h"

void broadcastState(const float distanceCm) {
    portENTER_CRITICAL(&gMux);
    const auto config = gConfig;
    portEXIT_CRITICAL(&gMux);

    JsonDocument document;
    document["distance"] = distanceCm;
    document["config"] = config;

    char buf[256];
    serializeJson(document, buf);
    events.send(buf, "state_update", millis());
}

static void handleConfig(AsyncWebServerRequest *request) {
    auto changed = false;

    if (request->hasParam("yellow", true)) {
        const auto value = request->getParam("yellow", true)->value().toFloat();
        if (value > 0 && value <= US100::MAX_DISTANCE_CM) {
            portENTER_CRITICAL(&gMux);
            gConfig.yellowThresholdCm = value;
            changed = true;
            portEXIT_CRITICAL(&gMux);
        }
    }

    if (request->hasParam("red", true)) {
        const auto value = request->getParam("red", true)->value().toFloat();
        if (value > 0 && value <= US100::MAX_DISTANCE_CM) {
            portENTER_CRITICAL(&gMux);
            gConfig.redThresholdCm = value;
            changed = true;
            portEXIT_CRITICAL(&gMux);
        }
    }

    if (request->hasParam("yellowVolume", true)) {
        const auto value = request->getParam("yellowVolume", true)->value().toInt();
        if (value > 0 && value <= UINT8_MAX) {
            portENTER_CRITICAL(&gMux);
            gConfig.yellowVolume = value;
            changed = true;
            portEXIT_CRITICAL(&gMux);
        }
    }

    if (request->hasParam("redVolume", true)) {
        const auto value = request->getParam("redVolume", true)->value().toInt();
        if (value > 0 && value <= UINT8_MAX) {
            portENTER_CRITICAL(&gMux);
            gConfig.redVolume = value;
            changed = true;
            portEXIT_CRITICAL(&gMux);
        }
    }

    if (changed) {
        broadcastState(gState.distanceCm);
        request->send(200, "application/json", R"("ok":true)");
    } else request->send(400, "application/json", R"({"ok":false, "error":"invalid params"})");
}

void setupRoutes() {
    server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
    server.on("/config", HTTP_POST, handleConfig);
    events.onConnect([](AsyncEventSourceClient *client) {
        portENTER_CRITICAL(&gMux);
        const auto distanceCm = gState.distanceCm;
        const auto config = gConfig;
        portEXIT_CRITICAL(&gMux);

        JsonDocument document;
        document["distance"] = distanceCm;
        document["config"] = config;

        char buf[256];
        serializeJson(document, buf);

        client->send(buf, "state_update", millis(), 1000);
    });
    server.addHandler(&events);
}
