#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>  // Include OTA Library
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define LED_PIN     D5
#define NUM_LEDS    72
#define JSON_BUFFER_SIZE 700

CRGB leds[NUM_LEDS];
CRGB targetLeds[NUM_LEDS];
uint8_t targetIntensity[NUM_LEDS];

const char* ssid = "DIDI";
const char* password = "";
IPAddress staticIP(10, 5, 0, 190);
IPAddress gateway(10, 5, 0, 1);
IPAddress subnet(255, 255, 252, 0);

WebSocketsServer webSocket = WebSocketsServer(81);
DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);  // Increased size to accommodate array data

void remotePrint(String msg) {
  for (int i = 0; i < webSocket.connectedClients(true); i++) {
    webSocket.sendTXT(i, msg);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      remotePrint("Disconnected!");
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        String msg = "Connected from: " + ip.toString();
        remotePrint(msg);
        Serial.println("WebSocket connection established from: " + ip.toString());
      }
      break;
    case WStype_TEXT:
      {
        // Print the received payload to the serial monitor
        Serial.print("Received message: ");
        for(int i = 0; i < length; i++) {
          Serial.print((char)payload[i]);
        }
        Serial.println();

        DeserializationError error = deserializeJson(jsonDoc, payload);
        if (error) {
          remotePrint("Failed to parse JSON");
          return;
        }
        JsonArray arr = jsonDoc.as<JsonArray>();
        for (int i = 0; i < arr.size() && i < NUM_LEDS; i++) {
          targetIntensity[i] = arr[i]; // Fixed color to yellow
        }
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  // fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());  // Print the SSID to which we are connected
  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());  // Print the local IP address

  // Initialize OTA
  ArduinoOTA.setHostname("Bob");
  ArduinoOTA.begin();
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
const float interpolationSpeed = 0.05; // Speed of interpolation, between 0 and 1

void loop() {
  webSocket.loop();
  ArduinoOTA.handle();  // Handle OTA updates

  // for (int i = 0; i < NUM_LEDS; i++) {
  //   uint8_t interpolatedIntensity = scale8(leds[i].getAverageLight(), targetIntensity[i]);
  //   leds[i].fadeToBlackBy(255 - interpolatedIntensity);
  // }
  for (int i = 0; i < NUM_LEDS; i++) {
    // Retrieve current and target intensity for each LED
    uint8_t currentIntensity = scale8(leds[i].getAverageLight(), 255);
    uint8_t newTargetIntensity = targetIntensity[i];

    // Interpolate between current and target intensity
    uint8_t interpolatedIntensity = currentIntensity + (newTargetIntensity - currentIntensity) * interpolationSpeed;

    // If the intensity has changed, update the LED
    if (interpolatedIntensity != currentIntensity) {
      leds[i] = CHSV(35, 255, interpolatedIntensity); // Yellow with dynamic intensity
    }
  }
  
  FastLED.show();
}
