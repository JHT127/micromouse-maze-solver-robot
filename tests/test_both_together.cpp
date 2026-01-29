// tests/test_both_together.cpp
// TEST BOTH MOTORS TOGETHER

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=== TEST BOTH MOTORS TOGETHER ===");
  Serial.println("Robot should move STRAIGHT");
  
  // Setup
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  // SAME speed (100/255 = 39% power)
  analogWrite(MOTOR_LEFT_PWM, 160);
  analogWrite(MOTOR_RIGHT_PWM, 110);
  
  delay(5000);
  Serial.println("\n>>> STARTING TEST <<<");
  
  // === FORWARD ===
  Serial.println("\n[FORWARD - 4 seconds]");
  Serial.println("Watch if robot moves STRAIGHT");
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(4000);
  
  // === STOP ===
  Serial.println("\n[STOP - 2 seconds]");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(2000);
  
  // === BACKWARD ===
  Serial.println("\n[BACKWARD - 4 seconds]");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, HIGH);
  delay(4000);
  
  // === FINAL STOP ===
  Serial.println("\n[STOP - FINAL]");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  Serial.println("\n=== TEST COMPLETE ===");
  Serial.println("Did robot move STRAIGHT?");
  Serial.println("Or did it curve/turn?");
}

void loop() {
  // Done
  delay(1000);
}