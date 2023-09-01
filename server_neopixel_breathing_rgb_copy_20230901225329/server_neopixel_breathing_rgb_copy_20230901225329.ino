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

unsigned long previousMillis = 0; 
const long interval = 50;  // interval to refresh LEDs in milliseconds
int brightness = 0;
int fadeAmount = 5;

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

  // Current time
  unsigned long currentMillis = millis();

  // Check if one interval has passed; note this will also work if millis() wraps
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if(webSocket.connectedClients() == 0) {
      // If no clients are connected, create a fading pattern
      brightness = brightness + fadeAmount;
      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      
      RgbColor lightRed(brightness, 0, 0);
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.SetPixelColor(i, lightRed);
      }
      strip.Show();
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    JsonArray array = doc.as<JsonArray>();

    // Get the RGB color from the first 3 values in the array
    uint8_t red = array[0];
    uint8_t green = array[1];
    uint8_t blue = array[2];
    RgbColor baseColor(red, green, blue);

    // Apply intensity to the LEDs starting from the 4th value in the array
    for (int i = 3; i < NUM_LEDS + 3; i++) {
      int intensity = array[i];
      RgbColor color = baseColor;
      color.Darken(255 - intensity); // Darken based on the intensity
      strip.SetPixelColor(i - 3, color);
    }

    // Update LEDs
    strip.Show();
  }
}

