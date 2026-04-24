// tests/calibrate_motors.cpp
// MOTOR CALIBRATION SCRIPT

#include <Arduino.h>
#include "../hardware/pins.h"

// Store calibrated values
int leftPWM = 250;
int rightPWM = 150;

void setupMotorPins() {
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  
  // Stop motors initially
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
}

void moveForward(int duration) {
  Serial.print("Moving forward with PWM: L=");
  Serial.print(leftPWM);
  Serial.print(" R=");
  Serial.println(rightPWM);
  
  analogWrite(MOTOR_LEFT_PWM, leftPWM);
  analogWrite(MOTOR_RIGHT_PWM, rightPWM);
  
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  
  delay(duration);
  
  // Stop
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("\n=== MOTOR CALIBRATION ===");
  Serial.println("Commands:");
  Serial.println("1. 'q' / 'a' : Increase/decrease LEFT motor PWM");
  Serial.println("2. 'w' / 's' : Increase/decrease RIGHT motor PWM");
  Serial.println("3. 't' : Test forward motion (2 seconds)");
  Serial.println("4. 'c' : Show current PWM values");
  Serial.println("5. 'd' : Default values (255)");
  Serial.println("6. 'x' : Exit calibration and show final values");
  
  setupMotorPins();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch(command) {
      case 'q':  // Increase left PWM
        if (leftPWM < 255) leftPWM += 1;
        Serial.print("Left PWM increased to: ");
        Serial.println(leftPWM);
        break;
        
      case 'a':  // Decrease left PWM
        if (leftPWM > 0) leftPWM -= 1;
        Serial.print("Left PWM decreased to: ");
        Serial.println(leftPWM);
        break;
        
      case 'w':  // Increase right PWM
        if (rightPWM < 255) rightPWM += 1;
        Serial.print("Right PWM increased to: ");
        Serial.println(rightPWM);
        break;
        
      case 's':  // Decrease right PWM
        if (rightPWM > 0) rightPWM -= 1;
        Serial.print("Right PWM decreased to: ");
        Serial.println(rightPWM);
        break;
        
      case 't':  // Test motion
        Serial.println("Testing forward motion...");
        moveForward(2000);
        break;
        
      case 'c':  // Current values
        Serial.print("Current PWM - Left: ");
        Serial.print(leftPWM);
        Serial.print(" Right: ");
        Serial.println(rightPWM);
        Serial.print("Ratio (L/R): ");
        Serial.println((float)leftPWM/rightPWM);
        break;
        
      case 'd':  // Default values
        leftPWM = 255;
        rightPWM = 255;
        Serial.println("Reset to default PWM (255)");
        break;
        
      case 'x':  // Exit
        Serial.println("\n=== CALIBRATION COMPLETE ===");
        Serial.print("Final values - Left: ");
        Serial.print(leftPWM);
        Serial.print(" Right: ");
        Serial.println(rightPWM);
        Serial.println("\nAdd these to motor_config.h:");
        Serial.print("const int CALIB_LEFT_PWM = ");
        Serial.print(leftPWM);
        Serial.println(";");
        Serial.print("const int CALIB_RIGHT_PWM = ");
        Serial.print(rightPWM);
        Serial.println(";");
        break;
        
      default:
        Serial.println("Unknown command. Use: q/a, w/s, t, c, d, x");
        break;
    }
  }
}