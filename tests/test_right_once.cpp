// tests/test_right_once.cpp
// TEST RIGHT MOTOR ONE TIME ONLY

#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("STEP 2: TEST RIGHT MOTOR ONCE");
  Serial.println("Watch RIGHT wheel only");
  
  // Setup
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  // Left motor OFF (keep correct)
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  
  // Same speed
  analogWrite(MOTOR_RIGHT_PWM, 100);
  
  Serial.println("\nRight motor will spin FORWARD for 5 seconds");
  Serial.println("Then STOP and wait");
  Serial.println("If backward, swap RIGHT motor wires");
  delay(5000);
  
  // === SINGLE TEST ===
  Serial.println(">>> STARTING RIGHT MOTOR NOW <<<");
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(5000);  // 5 seconds
  
  Serial.println(">>> STOPPED <<<");
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  Serial.println("\nTEST COMPLETE");
  Serial.println("Right motor forward? (Y/N)");
  Serial.println("Upload next test after this.");
}

void loop() {
  // Done
  delay(1000);
}