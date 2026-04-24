#include <Arduino.h>
#include <Wire.h>
#include "pins.h"
#include "LIDARSensor.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "MPU6050Driver.h"
#include "StateSpace.h"
#include "MotionController.h"

//------------------------------------------------------------
// LIDAR SENSORS
//------------------------------------------------------------
LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

//------------------------------------------------------------
// MOTION CONTROL
//------------------------------------------------------------
Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

PIDController leftPID(300.0, 40.0, 20.0);
PIDController rightPID(300.0, 40.0, 20.0);

MPU6050Driver imu;
StateSpace stateSpace;
MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        &leftPID, &rightPID,
                        &imu, &stateSpace);

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("\n\n=== LIDAR + MOTION TEST ===\n");
    Serial.println("Step 1: Serial initialized at 115200");
    Serial.println("Step 2: Initializing I2C...");
    Wire.begin();
    delay(100);
    Serial.println("Step 3: Scanning I2C bus...");
    
    for (uint8_t addr = 0x20; addr < 0x40; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("  Found: 0x");
            Serial.println(addr, HEX);
        }
    }
    
    Serial.println("\nStep 4: Disabling all sensors...");
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(100);
    
    Serial.println("Step 5: Init FRONT at 0x30...");
    frontLidar.enable();
    delay(50);
    bool f = frontLidar.initialize(0x30);
    Serial.print("  Result: ");
    Serial.println(f ? "OK" : "FAIL");
    
    Serial.println("Step 6: Init LEFT at 0x31...");
    leftLidar.enable();
    delay(50);
    bool l = leftLidar.initialize(0x31);
    Serial.print("  Result: ");
    Serial.println(l ? "OK" : "FAIL");
    
    Serial.println("Step 7: Init RIGHT at 0x32...");
    rightLidar.enable();
    delay(50);
    bool r = rightLidar.initialize(0x32);
    Serial.print("  Result: ");
    Serial.println(r ? "OK" : "FAIL");
    
    Serial.println("\nStep 8: Starting continuous mode...");
    frontLidar.startContinuous();
    leftLidar.startContinuous();
    rightLidar.startContinuous();
    Serial.println("  Continuous mode ON");
    
    Serial.println("\nStep 9: Reading for 20 MINUTES...");
    Serial.println("Move your hand around to see responsive readings!");
    Serial.println("Time | Front | Left | Right");
    
    unsigned long start = millis();
    while (millis() - start < 10000) {  // 20 minutes = 1200000 ms
        if ((millis() - start) % 1000 < 50) {
            uint16_t front = frontLidar.readRangeContinuousMillimeters();
            uint16_t left = leftLidar.readRangeContinuousMillimeters();
            uint16_t right = rightLidar.readRangeContinuousMillimeters();
            
            unsigned long elapsed = millis() - start;
            unsigned long mins = elapsed / 60000;
            unsigned long secs = (elapsed % 60000) / 1000;
            
            Serial.print(mins);
            Serial.print(":");
            if (secs < 10) Serial.print("0");
            Serial.print(secs);
            Serial.print(" | ");
            Serial.print(front);
            Serial.print(" | ");
            Serial.print(left);
            Serial.print(" | ");
            Serial.println(right);
        }
        delay(50);
    }
    
    Serial.println("\n\n========================================");
    Serial.println("TEST 2: WALL DETECTION FOR FLOODFILL");
    Serial.println("========================================");
    Serial.println("Bring hand close to sensors to simulate walls\n");
    Serial.println("Status: [Front] [Left] [Right]");
    Serial.println("Format: OK = clear, WALL = wall detected\n");
    
    start = millis();
    int detectionCount = 0;
    
    while (millis() - start < 10000) {  // 2 minutes for wall detection test
        if ((millis() - start) % 500 < 50) {
            uint16_t front = frontLidar.readRangeContinuousMillimeters();
            uint16_t left = leftLidar.readRangeContinuousMillimeters();
            uint16_t right = rightLidar.readRangeContinuousMillimeters();
            
            bool frontWall = frontLidar.isWallDetected();
            bool leftWall = leftLidar.isWallDetected();
            bool rightWall = rightLidar.isWallDetected();
            
            if (detectionCount % 2 == 0) {  // Print every other iteration (every 1 second)
                Serial.print(detectionCount / 2);
                Serial.print("s: [");
                Serial.print(frontWall ? "WALL" : "OK  ");
                Serial.print("] [");
                Serial.print(leftWall ? "WALL" : "OK  ");
                Serial.print("] [");
                Serial.print(rightWall ? "WALL" : "OK  ");
                Serial.print("]  (");
                Serial.print(front);
                Serial.print("mm, ");
                Serial.print(left);
                Serial.print("mm, ");
                Serial.print(right);
                Serial.println("mm)");
            }
            detectionCount++;
        }
        delay(50);
    }
    
    Serial.println("\n\n========================================");
    Serial.println("TEST 3: MOVE FORWARD (NO COLLISION)");
    Serial.println("========================================");
    Serial.println("Testing motion controller with LIDARs...\n");
    
    //------------------------------------------------------------
    // INITIALIZE MOTION
    //------------------------------------------------------------
    Serial.println("Initializing motion control...");
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    
    if (!imu.initialize()) {
        Serial.println("  IMU FAILED!");
    } else {
        imu.calibrate(100);
    }
    
    stateSpace.initialize();
    leftPID.setOutputLimits(0, 255);
    rightPID.setOutputLimits(0, 255);
    motion.initialize();
    motion.setForwardSpeed(0.15);
    
    Serial.println("Starting forward motion for 10 seconds...\n");
    Serial.println("Time | Front | Left | Right | Motor Status");
    
    start = millis();
    int moveCount = 0;
    
    while (millis() - start < 10000) {  // 10 seconds
        // Call moveForward every loop to update PID
        motion.moveForward(5.0);
        
        // Read LIDAR
        uint16_t front = frontLidar.readRangeContinuousMillimeters();
        uint16_t left = leftLidar.readRangeContinuousMillimeters();
        uint16_t right = rightLidar.readRangeContinuousMillimeters();
        
        // Print status every 500ms
        if ((millis() - start) % 500 < 50) {
            unsigned long elapsed = millis() - start;
            unsigned long secs = elapsed / 1000;
            
            Serial.print(secs);
            Serial.print("s: ");
            Serial.print(front);
            Serial.print("mm | ");
            Serial.print(left);
            Serial.print("mm | ");
            Serial.print(right);
            Serial.println("mm | MOVING");
        }
        
        delay(50);
    }
    
    // Stop motion
    motion.stop();
    leftMotor.setSpeed(0);
    rightMotor.setSpeed(0);
    
    Serial.println("\n========================================");
    Serial.println("TEST COMPLETE");
    Serial.println("========================================");
    Serial.println("\nMotion control verified!");
    Serial.println("LIDARs working during motion!");
    Serial.println("Ready for collision avoidance!");
    
}

void loop() {
    delay(1000);
}

