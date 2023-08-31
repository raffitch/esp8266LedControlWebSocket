#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#define LED_PIN     D5
#define NUM_LEDS    73

CRGB leds[NUM_LEDS];
CRGB targetLeds[NUM_LEDS];
uint8_t targetIntensity[NUM_LEDS];


const char* ssid = "DIDI";
const char* password = "";
IPAddress staticIP(10, 5, 2, 160);
IPAddress gateway(10, 5, 0, 1);
IPAddress subnet(255, 255, 252, 0);

WebSocketsServer webSocket = WebSocketsServer(81);
DynamicJsonDocument jsonDoc(1024);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      {
        DeserializationError error = deserializeJson(jsonDoc, payload);
        if (error) {
          Serial.println("Failed to parse JSON");
          return;
        }

        int cluster = jsonDoc["cluster"];
        int start = jsonDoc["start"];
        int end = jsonDoc["end"];
        int intensity = jsonDoc["intensity"];
        uint8_t ledR = jsonDoc["ledR"];
        uint8_t ledG = jsonDoc["ledG"];
        uint8_t ledB = jsonDoc["ledB"];
        
        Serial.printf("Received data for cluster %d: Start %d, End %d, Intensity %d, Color R %d G %d B %d\n", cluster, start, end, intensity, ledR, ledG, ledB);

        if (start >= 0 && end < NUM_LEDS && start <= end) {  // bounds check
          for (int i = start; i <= end; i++) {
            targetLeds[i] = CRGB(ledR, ledG, ledB);
            targetIntensity[i] = intensity;
          }
        }
      }
      break;
  }
}



void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);

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

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // Calculate the next LED states
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].r = lerp8by8(leds[i].r, targetLeds[i].r, 128);
    leds[i].g = lerp8by8(leds[i].g, targetLeds[i].g, 128);
    leds[i].b = lerp8by8(leds[i].b, targetLeds[i].b, 128);

    uint8_t interpolatedIntensity = scale8(leds[i].getAverageLight(), targetIntensity[i]);
    leds[i].fadeToBlackBy(255 - interpolatedIntensity);
  }
  
  FastLED.show();
}
