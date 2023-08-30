// Include necessary libraries
#include <ESP8266WiFi.h> // For WiFi functionality on ESP8266
#include <WebSocketsServer.h> // For WebSocket server functionality
#include <ESP8266mDNS.h> // For Multicast DNS (providing a local domain name)
#include <ArduinoJson.h> // For parsing JSON
#include <FastLED.h> // For controlling LED strip

// LED strip configuration
#define LED_PIN D5 // Data Pin connected to the LED strip
#define NUM_LEDS 1 // Number of LEDs in the strip
CRGB leds[NUM_LEDS]; // Create an array to hold LED data

// Create a JSON Document
DynamicJsonDocument jsonDoc(1024);

// WiFi Credentials
const char* ssid = "R-MB";
const char* password = "caroledubai";
int connectioncount = 0;
bool ready = true;

// Initialize WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Function to handle WebSocket events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: // Client disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: // Client connected
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      webSocket.sendTXT(num, "Connected");
      break;
    case WStype_TEXT: // Text message received
      ready = false; // Mark as not ready to prevent any other actions during this time
      
      // Deserialize received JSON payload
      DeserializationError error = deserializeJson(jsonDoc, payload);
      if (error) {
        Serial.println("Failed to parse JSON");
        return;
      }
      
      // Extract LED RGB values from JSON
      uint8_t ledR = jsonDoc["ledR"];
      uint8_t ledG = jsonDoc["ledG"];
      uint8_t ledB = jsonDoc["ledB"];
      
      // Uncomment the following line to set LED colors
      // fill_solid(leds, NUM_LEDS, CRGB(ledR, ledG, ledB));
      
      FastLED.show();
      FastLED.clear();
      
      jsonDoc.clear();
      ready = true;
      break;
  }
}

void setup() {
  // Initialize Serial and LED strip
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB::Red;
  FastLED.show();
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && connectioncount < 50) {
    ++connectioncount;
    delay(500);
    Serial.print(".");
  }
  
  // Configuring fixed IP if connected
  if (connectioncount < 50) {
    IPAddress Ip(172, 20, 10, 10);
    IPAddress Gateway(172, 20, 10, 1);
    IPAddress Subnet(255, 255, 255, 240);
    WiFi.config(Ip, Gateway, Subnet);
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nConnection error");
  }
  
  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  // Start mDNS service
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  MDNS.addService("ws", "tcp", 81);
}

void loop() {
  // WebSocket loop
  if (ready) {
    webSocket.loop();
  }
}
