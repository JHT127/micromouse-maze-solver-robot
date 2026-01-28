// tests/test_left_both.cpp
// TEST LEFT MOTOR BOTH DIRECTIONS

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("=== LEFT MOTOR BOTH DIRECTIONS ===");
  Serial.println("Watch LEFT wheel closely");
  
  // Setup
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  
  // Right motor OFF
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  // SLOW speed
  analogWrite(MOTOR_LEFT_PWM, 160);
  
  delay(5000);
  Serial.println("\n>>> STARTING TEST <<<");
  
  // === DIRECTION X ===
  Serial.println("\n[DIRECTION X - 4 seconds]");
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  delay(4000);
  
  Serial.println("[STOP - 3 seconds]");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  delay(3000);
  
  // === DIRECTION Y ===
  Serial.println("\n[DIRECTION Y - 4 seconds]");
  Serial.println("OPPOSITE direction");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
  delay(4000);
  
  Serial.println("[STOP - FINAL]");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  
  Serial.println("\n=== DONE ===");
  Serial.println("Which direction pushes robot forward?");
  Serial.println("X or Y?");
}

void loop() {
  // Done
  delay(1000);
}