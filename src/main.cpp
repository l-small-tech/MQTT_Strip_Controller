#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "RGBStrip.cpp"
#include "secrets.h"

// Long interrupts from the MQTT system cause the LED strip to flicker.
#define FASTLED_ALLOW_INTERRUPTS 0

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqttServer = MQTT_SERVER;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASS;
const int mqttPort = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);
RGBStrip strip;

byte macBytes[6];
String deviceName = "";
String homeTopic = "";
 
 
void callback(char* topic, byte* payload, unsigned int length) {
  // Announce the topic to the console
  Serial.printf("[%s]\t", topic);

  // Convert the payload to a string
  char* payloadStr = (char*)malloc(length + 1);
  for (int i = 0; i < length; i++) {
    payloadStr[i] = (char)payload[i];
  }
  Serial.print(payloadStr);
  Serial.println("\n");

  // Parse payload string into JSON object
  // DynamicJsonDocument doc(2048);
  StaticJsonDocument<2048> doc;
  deserializeJson(doc, payloadStr);


  // Set a specific pixel to an RGB color
  if (doc.containsKey("setPixel")) {
    uint8_t r, g, b;
    uint16_t i;

    i = doc["setPixel"]["i"];
    r = doc["setPixel"]["r"];
    g = doc["setPixel"]["g"];
    b = doc["setPixel"]["b"];

    strip.setPixelColor(i, r, g, b);
  }

  // Set all pixels to an RGB color
  if (doc.containsKey("setAll")) {
    uint8_t r, g, b;

    r = doc["setAll"]["r"];
    g = doc["setAll"]["g"];
    b = doc["setAll"]["b"];

    strip.setStripColor(r, g, b);
  }

  // Clean up memory
  doc.clear();
  delete payloadStr;
  
  // print heap size
  Serial.print("Free memory: ");
  Serial.println(ESP.getFreeHeap());
}

void setup() {
 
  Serial.begin(115200);
  Serial.println("\n");
 
  // Connect to WiFi
  Serial.print("Connecting to WiFi.");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // What is my name? It's your MAC address :)
  WiFi.macAddress(macBytes);
  for (int i = 3; i < 6; i++) { deviceName += String(macBytes[i], HEX); }
  Serial.printf("Hello! My name is %s\n", deviceName);
 
  // Connect to MQTT Server
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.print("Connecting to MQTT.");

  while (!client.connected()) {
    
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println(".Connected!");  
 
    } else {

      Serial.print(".Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  Serial.println();
 
  // Tell the network we're online
  homeTopic = "esp/" + deviceName;
  client.publish(homeTopic.c_str(), "online");
  // Subscribe to all messages aimed at this device
  String subscribeTopic = homeTopic + "/#";
  client.subscribe(subscribeTopic.c_str(), 0);
}
 
void loop() {
  client.loop();
  strip.loopHook();
}
