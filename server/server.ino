/**
 * @file        LED_Controller_Arduino.ino
 * @author      [Raffi Tchakerian]
 * @date        [2023-08-30]
 * @version     [1.0]
 * 
 * @brief       This program is an Arduino sketch designed to run on an ESP8266.
 *              It creates a WebSocket server to receive JSON data for controlling LEDs.
 *              Each LED cluster's color and intensity can be controlled independently.
 * 
 * @note        Libraries required: ESP8266WiFi, WebSocketsServer, ArduinoJson
 * 
 */

// Import required libraries
#include <ESP8266WiFi.h>     // WiFi library for ESP8266
#include <WebSocketsServer.h> // WebSocket server library | search and install websockets from libraries
#include <ArduinoJson.h>      // Library to handle JSON data | search and install ArduinoJson from libraries
#include <FastLED.h>          // LED control library

// Constants to define LED properties
#define LED_PIN     D5        // Pin where the LED data line is connected
#define NUM_LEDS    16        // Number of LEDs in the strip
CRGB leds[NUM_LEDS];          // Array to store the state of each LED

// WiFi Credentials - !!IMPORTANT WILL ONLY WORK WITH 2.4GHZ WIFI SIGNALS NOT 5GHZ
const char* ssid = "DIDI";               // WiFi network name
const char* password = "";        // WiFi network password
// Network Configuration
IPAddress staticIP(10, 5, 2, 160); // Static IP for the ESP8266
IPAddress gateway(10, 5, 0, 1);    // Network Gateway (usually your router IP)
IPAddress subnet(255, 255, 252, 0);   // Network Subnet Mask

// WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);
// JSON Document to store incoming JSON payload
DynamicJsonDocument jsonDoc(1024);

// Function to handle WebSocket events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);  // When a client disconnects
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);   // Get client's IP address
        // Log connection and inform client
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "Connected");  // Send a "Connected" message to the client
      }
      break;
    case WStype_TEXT:
      {
        // Deserialize incoming JSON payload
        DeserializationError error = deserializeJson(jsonDoc, payload);
        if (error) {
          Serial.println("Failed to parse JSON");  // If the JSON is invalid
          return;
        }
        
        // Extract data from JSON
        int cluster = jsonDoc["cluster"];    // LED cluster to control
        int intensity = jsonDoc["intensity"]; // Brightness level
        uint8_t ledR = jsonDoc["ledR"]; // Red component
        uint8_t ledG = jsonDoc["ledG"]; // Green component
        uint8_t ledB = jsonDoc["ledB"]; // Blue component

        int start = 0;
        int end = 0;

        // Define which LEDs are in each cluster
        if (cluster == 1) { start = 0; end = 25; }
        if (cluster == 2) { start = 26; end = 50; }
        if (cluster == 3) { start = 51; end = 73; }

        // Update LEDs based on incoming JSON data
        for (int i = start; i <= end; i++) {
          leds[i] = CRGB(ledR, ledG, ledB);          // Set color
          leds[i].fadeToBlackBy(255 - intensity);    // Adjust brightness
        }
        
        FastLED.show();  // Update the actual LED strip
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  
  // Initialize FastLED library
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);  // Assign static IP details
  
  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  
  // Start WebSocket server and register event handler function
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // Keep the WebSocket server running
  webSocket.loop();
}
