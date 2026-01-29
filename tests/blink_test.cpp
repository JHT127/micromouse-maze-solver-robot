#include <Arduino.h>

// Built-in LED pin 
#define LED_PIN 2

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  delay(1000); // Wait for Serial to initialize
  
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);
  
  // Print startup message
  Serial.println("==================================");
  Serial.println("ESP32 Micromouse Test");
  Serial.println("Built-in LED should blink");
  Serial.println("==================================");
}

void loop() {
  // Turn LED ON
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000); // Wait 1 second
  
  // Turn LED OFF
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000); // Wait 1 second
  
  // Optional: Fast blink to show it's alive
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}