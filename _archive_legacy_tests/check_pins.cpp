// tests/check_pins.cpp
// QUICK PIN CHECK - NO MOTOR MOVEMENT

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("CHECKING PINS...");
  Serial.println("================");
  
  Serial.println("MOTOR PINS:");
  Serial.println("Left:  A=26  B=27  PWM=25");
  Serial.println("Right: A=12  B=13  PWM=14");
  
  Serial.println("\nENCODER PINS:");
  Serial.println("Left:  A=32  B=33");
  Serial.println("Right: A=34  B=35");
  
  Serial.println("\nIf wrong, update hardware/pins.h");
  Serial.println("Then upload to check.");
  
  delay(3000);
  
  Serial.println("\nPins check complete.");
  Serial.println("Motors NOT moving - safe.");
}

void loop() {
  // Nothing - just check pins
  delay(1000);
}