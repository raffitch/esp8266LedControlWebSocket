#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

// WiFi settings
const char* ssid = "DIDI";
const char* password = "";
IPAddress staticIP(10, 5, 0, 190);
IPAddress gateway(10, 5, 0, 1);
IPAddress subnet(255, 255, 252, 0);

// FastLED settings
#define NUM_LEDS 72
#define DATA_PIN D5
CRGB leds[NUM_LEDS];
int currentIntensity[NUM_LEDS];
int targetIntensity[NUM_LEDS];

// Create a WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize FastLED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Initialize current and target intensity arrays
  for (int i = 0; i < NUM_LEDS; i++) {
    currentIntensity[i] = 0;
    targetIntensity[i] = 0;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Use static IP
  WiFi.config(staticIP, gateway, subnet);

  Serial.println("Connected to WiFi");
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());  // Print the SSID to which we are connected
  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP()); 

  // Start the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // Interpolate towards the target intensities
  for (int i = 0; i < NUM_LEDS; i++) {
    if (currentIntensity[i] != targetIntensity[i]) {
      if (currentIntensity[i] < targetIntensity[i]) {
        currentIntensity[i]++;
      } else {
        currentIntensity[i]--;
      }
      leds[i] = CHSV(35, 255, currentIntensity[i]);
      FastLED.show();
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    JsonArray array = doc.as<JsonArray>();
    for (int i = 0; i < NUM_LEDS; i++) {
      targetIntensity[i] = array[i];
    }
  }
}
