#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

const char* wifi_name = "Laboo";
const char* wifi_pass = "laboo2025";

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("\n=== Micromouse Starting ===");
  
  // Connect to WiFi
  WiFi.begin(wifi_name, wifi_pass);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Configure OTA
    ArduinoOTA.setPort(8266);
    ArduinoOTA.setPassword("laboo2025");
    ArduinoOTA.setHostname("Laboo");
    
    ArduinoOTA.onStart([]() {
      Serial.println("OTA Update Starting...");
    });
    
    ArduinoOTA.onEnd([]() {
      Serial.println("\nOTA Update Complete!");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress * 100) / total);
    });
    
    ArduinoOTA.begin();
    //Serial.println("✓ OTA Ready on port 8266");
    Serial.println("✓ OTA Ready - TEST WIRELESS UPLOAD!");
    
  } else {
    Serial.println("\n✗ WiFi Failed! Check credentials");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
  delay(10);
  Serial.println("Waiting for wireless upload...");
}