// tests/test_right_slow.cpp
// SLOW TEST - RIGHT MOTOR BOTH DIRECTIONS

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=== SLOW RIGHT MOTOR TEST ===");
  Serial.println("SLOW speed - Watch closely");
  
  // Setup
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  // Left motor OFF
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  
  // VERY SLOW speed (40/255 = 16% power)
  analogWrite(MOTOR_RIGHT_PWM, 100);
  
  delay(5000);
  Serial.println("\n>>> STARTING TEST <<<");
  
  // === DIRECTION A (SLOW) ===
  Serial.println("\n[DIRECTION A - 4 seconds]");
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(4000);  // 4 seconds
  
  Serial.println("[STOP - 3 seconds]");
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(3000);
  
  // === DIRECTION B (SLOW) ===
  Serial.println("\n[DIRECTION B - 4 seconds]");
  Serial.println("OPPOSITE direction");
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, HIGH);
  delay(4000);  // 4 seconds
  
  Serial.println("[STOP - FINAL]");
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  Serial.println("\n=== DONE ===");
  Serial.println("Which direction pushes robot forward?");
  Serial.println("A or B?");
}

void loop() {
  // Done
  delay(1000);
}