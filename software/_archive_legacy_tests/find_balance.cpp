// tests/find_balance.cpp
#include <Arduino.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  Serial.println("Finding balance...");
  Serial.println("We'll increase LEFT motor until both are equal");
  
  int rightPWM = 150;  // Keep right constant
  
  // Test different left PWM values
  for(int leftPWM = 150; leftPWM <= 255; leftPWM += 20) {
    Serial.print("\nTesting: Left=");
    Serial.print(leftPWM);
    Serial.print(" Right=");
    Serial.println(rightPWM);
    
    analogWrite(MOTOR_LEFT_PWM, leftPWM);
    analogWrite(MOTOR_RIGHT_PWM, rightPWM);
    
    digitalWrite(MOTOR_LEFT_A, HIGH);
    digitalWrite(MOTOR_LEFT_B, LOW);
    digitalWrite(MOTOR_RIGHT_A, HIGH);
    digitalWrite(MOTOR_RIGHT_B, LOW);
    
    delay(3000);  // Watch robot for 3 seconds
    
    // Stop
    digitalWrite(MOTOR_LEFT_A, LOW);
    digitalWrite(MOTOR_LEFT_B, LOW);
    digitalWrite(MOTOR_RIGHT_A, LOW);
    digitalWrite(MOTOR_RIGHT_B, LOW);
    delay(2000);
  }
  
  Serial.println("\nWhich test moved STRAIGHTEST?");
  Serial.println("Tell me the leftPWM value that worked best.");
}

void loop() {}