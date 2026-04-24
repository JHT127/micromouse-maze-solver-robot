
#include <Arduino.h>
#include "pins.h"
#include "MPU6050Driver.h"

//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: MPU6050 sensor
//------------------------------------------------------------

MPU6050Driver imu;


//------------------------------------------------------------
// SETUP
// Purpose: Initialize and run tests
//------------------------------------------------------------

void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MPU6050 GYROSCOPE TEST");
    Serial.println("========================================\n");
    
    // Test 1: Initialize
    Serial.println("========================================");
    Serial.println("TEST 1: Initialization");
    Serial.println("========================================");
    
    if (!imu.initialize()) {
        Serial.println("✗ FAILED: MPU6050 not found!");
        Serial.println("  Check I2C connections");
        Serial.println("  Verify pullup resistors on SDA/SCL");
        while(1) delay(1000);
    }
    Serial.println("✓ MPU6050 found and initialized\n");
    
    // Test 2: Calibration
    Serial.println("========================================");
    Serial.println("TEST 2: Calibration");
    Serial.println("========================================");
    Serial.println("Keep robot COMPLETELY STILL for 5 seconds...");
    
    for (int i = 5; i > 0; i--) {
        Serial.print(i);
        Serial.println("...");
        delay(1000);
    }
    
    if (!imu.calibrate(500)) {
        Serial.println("✗ FAILED: Calibration error");
        while(1) delay(1000);
    }
    Serial.println("✓ Calibration complete\n");
    
    // Test 3: Angle tracking
    Serial.println("========================================");
    Serial.println("TEST 3: Angle Tracking");
    Serial.println("========================================");
    Serial.println("Wait 2 seconds, then slowly rotate robot");
    Serial.println("You should see angle increase\n");
    
    delay(2000);
}


//------------------------------------------------------------
// LOOP
// Purpose: Read and display gyroscope data
//------------------------------------------------------------

void loop() {
    float dt = 0.02;  // 20ms
    
    imu.update(dt);
    
    Serial.print("Angle: ");
    Serial.print(imu.getAngleDegrees(), 2);
    Serial.print("°  Angular Vel: ");
    Serial.print(imu.getAngularVelocity(), 3);
    Serial.println(" rad/s");
    
    delay(20);
}
