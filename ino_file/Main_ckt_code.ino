#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Ak";  
const char* password = "Ak88888888";

WebServer server(80);  // HTTP server on port 80

// Pin assignments
#define DHTPIN 4        // DHT22 on pin D4
#define DHTTYPE DHT22   
#define MOISTUREPIN 34  // Soil moisture on pin D34
#define RELAYPIN 13     // Relay on pin D13

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    
    // Initialize hardware
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW);  // Start with relay OFF
    pinMode(MOISTUREPIN, INPUT);
    dht.begin();

    // Connect to WiFi with timeout
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi!");
        while(1) delay(1000);  // Hang if connection fails
    } else {
        Serial.println("\nConnected to Wi-Fi!");
        Serial.print("ESP32 IP Address: ");
        Serial.println(WiFi.localIP());
    }

    // Set up server endpoints
    server.on("/sensor-data", handleSensorData);
    
  // Autonomous Pump Control Endpoint
server.on("/auto-pump", []() {
    // Enable CORS
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    // Read soil moisture (0-100%)
    int moisture = analogRead(MOISTUREPIN);
    int moisturePercent = map(moisture, 0, 4095, 100, 0);
    
    // Automatic control logic
    bool pumpState = (moisturePercent < 50); // Threshold
    digitalWrite(RELAYPIN, pumpState ? HIGH : LOW);
    
    // JSON response
    String json = "{";
    json += "\"moisture\":" + String(moisturePercent) + ",";
    json += "\"pump_state\":" + String(pumpState);
    json += "}";
    
    server.send(200, "application/json", json);
    
    // Debug logs
    Serial.printf("[AUTO] Moisture: %d%% -> Pump %s\n", 
                 moisturePercent, pumpState ? "ON" : "OFF");
});
    server.begin();
    Serial.println("HTTP server started");
}

void handleSensorData() {
    // Read sensors with error handling
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    if (isnan(temperature) || isnan(humidity)) {
        server.send(500, "text/plain", "Failed to read DHT sensor");
        Serial.println("DHT sensor error!");
        return;
    }

    int moisture = analogRead(MOISTUREPIN);
    int moisturePercentage = map(moisture, 0, 4095, 100, 0);  // Convert to %
    int relayStatus = digitalRead(RELAYPIN);

    // Create JSON response
    String json = "{";
    json += "\"temperature\":" + String(temperature, 2) + ",";
    json += "\"humidity\":" + String(humidity, 2) + ",";
    json += "\"moisture\":" + String(moisturePercentage) + ",";
    json += "\"relay_status\":" + String(relayStatus) + ",";
    json += "\"light\":300,";  // Dummy values
    json += "\"ph\":7,";
    json += "\"nitrogen\":120,";
    json += "\"phosphorus\":60,";
    json += "\"potassium\":150";
    json += "}";

    server.send(200, "application/json", json);
    Serial.println("Sent sensor data to client");
}

void loop() {
    server.handleClient();  // Handle incoming requests
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) {
        lastCheck = millis();
        
        // Trigger auto-pump logic
        int moisture = analogRead(MOISTUREPIN);
        int moisturePercent = map(moisture, 0, 4095, 100, 0);
        bool pumpState = (moisturePercent < 50);
        digitalWrite(RELAYPIN, pumpState ? HIGH : LOW);
        
        Serial.printf("[AUTO] %ds Check: Moisture=%d%%, Pump=%d\n",
                     millis()/1000, moisturePercent, pumpState);
    }
    
    // WiFi reconnection logic (added improvement)
    static unsigned long lastWifiCheck = 0;
    if (millis() - lastWifiCheck > 5000) {  // Check every 5 seconds
        lastWifiCheck = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected! Reconnecting...");
            WiFi.reconnect();
            delay(1000);  // Wait 1 second for connection
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("WiFi reconnected!");
            }
        }
    }
}