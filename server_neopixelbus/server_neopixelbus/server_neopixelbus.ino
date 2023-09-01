#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <NeoPixelBus.h>

// WiFi settings
const char* ssid = "Linksys";
const char* password = "1q2w3e4r5T";
IPAddress staticIP(192, 168, 0, 88);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// NeoPixelBus settings
#define NUM_LEDS 72

// Using DMA method; this is specific to GPIO3 (RX)
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(NUM_LEDS);

// Create a WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);


void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize NeoPixelBus
  strip.Begin();
  strip.Show();

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
  Serial.println(WiFi.SSID());
  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());

  // Start the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  if(webSocket.connectedClients() == 0) {
    // If no clients are connected, set all LEDs to light red
    RgbColor lightRed(255, 128, 128); // Adjust as needed
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.SetPixelColor(i, lightRed);
    }
    strip.Show();
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    JsonArray array = doc.as<JsonArray>();
    for (int i = 0; i < NUM_LEDS; i++) {
      int intensity = array[i];
      RgbColor color(255, 255, 0); // Yellow color
      color.Darken(255 - intensity); // Darken based on the intensity
      strip.SetPixelColor(i, color);
    }

    // Update LEDs
    strip.Show();
  }
}
