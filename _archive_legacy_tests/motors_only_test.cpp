// tests/motors_only_test.cpp
// TEST MOTORS ONLY - NO ENCODERS

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  delay(3000);  // Time to read
  
  Serial.println("=== MOTORS ONLY TEST ===");
  Serial.println("Testing L298N motor driver");
  Serial.println("Encoders NOT used in this test");
  
  // Setup ONLY motor pins
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  
  // Setup PWM pins (speed control)
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  // Set MEDIUM speed (150/255 = 59% power)
  analogWrite(MOTOR_LEFT_PWM, 150);
  analogWrite(MOTOR_RIGHT_PWM, 150);
  
  Serial.println("\nTEST SEQUENCE:");
  Serial.println("1. Forward 2 sec");
  Serial.println("2. Stop 2 sec");
  Serial.println("3. Right turn 2 sec");
  Serial.println("4. Stop 2 sec");
  Serial.println("5. Left turn 2 sec");
  Serial.println("6. Stop 5 sec (end)");
  
  delay(3000);
  Serial.println("\nStarting...");
}

void loop() {
  // 1. FORWARD
  Serial.println(">>> FORWARD");
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(2000);
  
  // 2. STOP
  Serial.println(">>> STOP");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(2000);
  
  // 3. TURN RIGHT (left forward, right backward)
  Serial.println(">>> TURN RIGHT");
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, HIGH);
  delay(2000);
  
  // 4. STOP
  Serial.println(">>> STOP");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(2000);
  
  // 5. TURN LEFT (left backward, right forward)
  Serial.println(">>> TURN LEFT");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(2000);
  
  // 6. FINAL STOP (long pause)
  Serial.println(">>> FINAL STOP - TEST COMPLETE");
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(5000);
  
  Serial.println("\n=== RESTARTING TEST ===");
}