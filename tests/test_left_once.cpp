// tests/test_left_obvious.cpp
// OBVIOUS LEFT MOTOR TEST

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("=== OBVIOUS LEFT MOTOR TEST ===");
  Serial.println("Put MARK on LEFT wheel");
  Serial.println("Watch MARK direction");
  
  // Setup
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  
  // Right motor OFF
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  // Higher speed for visibility
  analogWrite(MOTOR_LEFT_PWM, 150);
  
  Serial.println("\n=== TEST STARTING ===");
  Serial.println("Watch MARK on LEFT wheel");
  Serial.println("FORWARD = Mark moves AWAY from you");
  Serial.println("BACKWARD = Mark moves TOWARD you");
  delay(5000);
  
  Serial.println("\n>>> SPINNING LEFT MOTOR NOW <<<");
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  delay(5000);  // 5 seconds to observe
  
  Serial.println(">>> STOPPED <<<");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  
  Serial.println("\n=== DIRECTION RESULT ===");
  Serial.println("Which way did MARK move?");
  Serial.println("1. AWAY from you = FORWARD ✓");
  Serial.println("2. TOWARD you = BACKWARD ✗ (swap wires)");
  Serial.println("3. NOT MOVING = No power/connection");
  
  Serial.println("\nUpload new test after fixing.");
}

void loop() {
  // Done
  delay(1000);
}