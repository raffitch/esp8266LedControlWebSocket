#include <ESP8266WiFi.h> //Load ESP8266 Libs
#include <WebSocketsServer.h> // Load WS Libs
#include <ESP8266mDNS.h> //I think this is needed to have a static IP, cause you can also set it dynamic
#include <ArduinoJson.h> //This is for the Arduino Json

#include <FastLED.h> //Library for our targetable led strip

//Define things for LED strip
#define LED_PIN     D5//Led pin D7 on the ESP8266
#define NUM_LEDS    1//No. of Leds
CRGB leds[NUM_LEDS];

DynamicJsonDocument jsonDoc(1024);
 //We have to check this because you size it based on the amount of info you're sending check here https://arduinojson.org/v5/assistant/

const char* ssid = "R-MB"; //Name of wifi to connect to
const char* password = "caroledubai"; //password
int connectioncount = 0;
bool ready = true;

WebSocketsServer webSocket = WebSocketsServer(81);

//When there is an incoming websocket, it's percieved as an event and the loop is interrupted to execute the below
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
        // Print the payload when connected
        Serial.printf("Payload: ");
        for (size_t i = 0; i < length; i++) {
          Serial.printf("%c", (char) payload[i]);
        }
        Serial.println();

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      {
        ready = false;
        Serial.println();
        
        DeserializationError error = deserializeJson(jsonDoc, payload);
        
        if (error) {
          Serial.println("Failed to parse JSON");
          return;
        }
        
        uint8_t ledR = jsonDoc["ledR"];
        uint8_t ledG = jsonDoc["ledG"];
        uint8_t ledB = jsonDoc["ledB"];
        
        Serial.printf("ledR = %d\r\n", ledR);
        Serial.printf("ledG = %d\r\n", ledG);
        Serial.printf("ledB = %d\r\n", ledB);
        
        // fill_solid(leds, NUM_LEDS, CRGB(ledR, ledG, ledB));
        
        FastLED.show();
        FastLED.clear();
        
        jsonDoc.clear(); 
        ready = true;
      }
      break;

  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB::Red; // Set initial color to red to debug
  FastLED.show(); 
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and connectioncount < 50) {
    ++connectioncount;
    delay(500);
    Serial.print(".");
  }
  if (connectioncount < 50) {
    //To use a fixed ip
    IPAddress Ip(172, 20, 10, 10);
    IPAddress Gateway(172, 20, 10, 1);
    IPAddress Subnet(255, 255, 255, 240);
    WiFi.config(Ip, Gateway, Subnet);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection error");
  }


  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  MDNS.addService("ws", "tcp", 81);
}

void loop() {
  if (ready) {
    webSocket.loop();
  }
}