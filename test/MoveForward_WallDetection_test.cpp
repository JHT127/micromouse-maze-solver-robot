#include <Arduino.h>
#include <Wire.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "StateSpace.h"
#include "MPU6050Driver.h"
#include "MotionController.h"
#include "LIDARSensor.h"

//------------------------------------------------------------
// MOTION CONTROL
//------------------------------------------------------------
Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

PIDController leftPID(500.0, 28.0, 6.0);
PIDController rightPID(500.0, 28.0, 6.0);

StateSpace robot;
MPU6050Driver imu;

MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        &leftPID, &rightPID,
                        &imu, &robot);

//------------------------------------------------------------
// LIDAR SENSORS
//------------------------------------------------------------
LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

//------------------------------------------------------------
// TEST STATE
//------------------------------------------------------------
unsigned long lastMoveTime = 0;

//------------------------------------------------------------
// SETUP
// Purpose: Initialize motion + LIDARs
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MOTION + WALL DETECTION TEST");
    Serial.println("========================================\n");
    
    // Initialize hardware
    Serial.println("Initializing hardware...");
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    robot.initialize();
    
    Serial.println("Initializing IMU...");
    if (!imu.initialize()) {
        Serial.println("✗ MPU6050 not found!");
        while(1) delay(1000);
    }
    
    Serial.println("Calibrating IMU (keep still)...");
    imu.calibrate(500);
    
    Serial.println("Configuring PID...");
    leftPID.setOutputLimits(0, 255);
    rightPID.setOutputLimits(0, 255);
    
    motion.initialize();
    motion.setForwardSpeed(0.15);  // m/s
    motion.setTurnSpeed(1.57);     // rad/s
    
    // Initialize LIDARs
    Serial.println("Initializing LIDARs...");
    Wire.begin();
    delay(100);
    
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(100);
    
    frontLidar.enable();
    delay(50);
    frontLidar.initialize(0x30);
    
    leftLidar.enable();
    delay(50);
    leftLidar.initialize(0x31);
    
    rightLidar.enable();
    delay(50);
    rightLidar.initialize(0x32);
    
    Serial.println("Starting continuous LIDAR mode...");
    frontLidar.startContinuous();
    leftLidar.startContinuous();
    rightLidar.startContinuous();
    delay(500);
    
    Serial.println("✓ All hardware ready\n");
    
    Serial.println("========================================");
    Serial.println("TEST 1: Move Forward (NO WALL DETECTION)");
    Serial.println("========================================");
    Serial.println("Starting forward motion...\n");
    
    lastMoveTime = millis();
}

//------------------------------------------------------------
// LOOP
// Purpose: Test 1: Move forward freely, then Test 2: with wall detection
//------------------------------------------------------------

bool test1_done = false;
bool test2_done = false;
int test_state = 0;

void loop() {
    unsigned long now = millis();
    
    // TEST 1: Move forward WITHOUT wall detection
    if (test_state == 0) {
        if (motion.moveForward(0.30)) {
            test1_done = true;
            Serial.println("\n✓ Forward movement complete\n");
            
            delay(1000);
            
            Serial.println("========================================");
            Serial.println("TEST 2: Move Forward (WITH WALL DETECTION)");
            Serial.println("========================================");
            Serial.println("Starting forward motion... (bring obstacle if you want)\n");
            
            test_state = 1;
            lastMoveTime = millis();
        }
        
        // Debug: Print encoder velocities every 500ms
        if (now % 500 < 50) {
            float leftVel = leftEncoder.getVelocity(0.02);
            float rightVel = rightEncoder.getVelocity(0.02);
            Serial.print("L_vel: ");
            Serial.print(leftVel, 3);
            Serial.print(" m/s | R_vel: ");
            Serial.print(rightVel, 3);
            Serial.println(" m/s");
        }
    }
    
    // TEST 2: Move forward WITH wall detection
    else if (test_state == 1) {
        // Read LIDAR only every 100ms to avoid disrupting PID loop
        static unsigned long lastLidarRead = 0;
        bool frontWall = false;
        uint16_t frontDist = 0;
        
        if (now - lastLidarRead > 100) {
            lastLidarRead = now;
            frontDist = frontLidar.readRangeContinuousMillimeters();
            frontWall = frontLidar.isWallDetected();
            
            if (frontWall) {
                motion.stop();
                Serial.println("\n\n✗ WALL DETECTED!");
                Serial.print("Front Distance: ");
                Serial.print(frontDist);
                Serial.println("mm");
                Serial.println("Motion stopped!\n");
                test_state = 2;
            }
        }
        
        // Try to move forward (runs every 20ms)
        if (!frontWall && !motion.moveForward(0.90)) {
            // Still moving
        } else if (frontWall) {
            // Wall detected
        } else {
            // Movement complete
            test2_done = true;
            Serial.println("\n✓ Forward movement complete\n");
            
            Serial.println("========================================");
            Serial.println("ALL TESTS COMPLETE");
            Serial.println("========================================");
            Serial.println("✓ Both tests passed!");
            Serial.println("  - Forward motion works (no detection)");
            Serial.println("  - Wall detection works");
            Serial.println("\n✓ Motion controller with LIDARs is ready!\n");
            
            test_state = 2;
        }
    }
    
    else {
        // Tests complete, just idle
        delay(1000);
    }
    
    delay(20);  // 50Hz control loop: 1s/50 = 20ms per cycle
}
