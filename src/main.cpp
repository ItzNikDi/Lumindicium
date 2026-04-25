#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "display.h"
#include <US100.h>
#include <WiFi.h>
#include <esp_http_server.h>

US100 sonar(Serial2);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

httpd_handle_t server = NULL;

// shared sensor value
float lastMeasurement = 0;
bool hasValidMeasurement = false;

// ===== SENSOR TASK (non-blocking replacement for loop delays)
void sensorTask(void* parameter) {
    while (true) {
        const auto measurement = sonar.readDistanceCm();

        Serial.printf("Read value: %.2f\n", measurement);

        if (measurement != US100::ERROR_VALUE) {
            lastMeasurement = measurement;
            hasValidMeasurement = true;

            LCD.fillScreen(TFT_BLACK);
            LCD.setCursor(20, 30);
            LCD.printf("Distance: %.2f cm", measurement);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// ===== SSE HANDLER
esp_err_t sse_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/event-stream");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");

    while (true) {
        if (hasValidMeasurement) {
            String json = "{\"distance\": " + String(lastMeasurement) + "}";
            String msg = "data: " + json + "\n\n";

            if (httpd_resp_send_chunk(req, msg.c_str(), msg.length()) != ESP_OK) {
                break; // client disconnected
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    return ESP_OK;
}

// ===== STATIC FILE HANDLER
esp_err_t file_handler(httpd_req_t *req) {
    String path = req->uri;
    if (path == "/") path = "/index.html";

    File file = LittleFS.open(path, "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";

    httpd_resp_set_type(req, contentType.c_str());

    char buffer[1024];
    while (file.available()) {
        size_t len = file.readBytes(buffer, sizeof(buffer));
        httpd_resp_send_chunk(req, buffer, len);
    }

    httpd_resp_send_chunk(req, NULL, 0);
    file.close();

    return ESP_OK;
}

// ===== SERVER SETUP
void startServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);

    httpd_uri_t sse_uri = {
        .uri = "/events",
        .method = HTTP_GET,
        .handler = sse_handler
    };

    httpd_uri_t file_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = file_handler
    };

    httpd_register_uri_handler(server, &sse_uri);
    httpd_register_uri_handler(server, &file_uri);
}

void setup() {
#if defined(ARDUINO_M5STACK_Core2)
    auto config = m5::M5Unified::config();
    config.serial_baudrate = 115200;
    M5.begin(config);
#elif defined(ARDUINO_ESP32_DEV)
    Serial.begin(115200);
#endif

    Serial2.begin(9600, SERIAL_8N1, 13, 14);

    initDisplay();

    if (sonar.begin()) {
        Serial.printf("Sonar initialized!\n");
        LCD.printf("Sonar initialized!\n");
    }

    // WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.println(WiFi.localIP());

    // filesystem
    LittleFS.begin(true);

    // server
    startServer();

    // start sensor task (instead of loop delays)
    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorTask",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

void loop() {
    // empty: everything runs in tasks
}