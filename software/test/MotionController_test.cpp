#include <Arduino.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "StateSpace.h"
#include "MPU6050Driver.h"
#include "MotionController.h"

//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: All components for motion control
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

unsigned long lastMoveTime = 0;

//------------------------------------------------------------
// SETUP
// Purpose: Initialize everything and run tests
//------------------------------------------------------------

void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MOTION CONTROLLER TEST");
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
    
    Serial.println("✓ All hardware ready\n");
    
    Serial.println("========================================");
    Serial.println("TEST 1: Move Forward 20cm");
    Serial.println("========================================");
    Serial.println("Starting forward motion...\n");
    
    lastMoveTime = millis();
}


//------------------------------------------------------------
// LOOP
// Purpose: Manually orchestrate motion tests
//------------------------------------------------------------

bool test1_done = false;
bool test2_done = false;
bool test3_done = false;
int test_state = 0;

void loop() {
    unsigned long now = millis();
    
    // TEST 1: Move forward
    if (test_state == 0) {
        if (motion.moveForward(0.20)) {
            test1_done = true;
            Serial.println("\n✓ Forward movement complete\n");
            
            delay(1000);
            
            Serial.println("========================================");
            Serial.println("TEST 2: Turn Left 90 degrees");
            Serial.println("========================================");
            Serial.println("Starting left turn...\n");
            
            test_state = 1;
            lastMoveTime = millis();
        }
    }
    
    // TEST 2: Turn left
    else if (test_state == 1) {
        if (motion.turnLeft(PI / 2)) {
            test2_done = true;
            Serial.println("\n✓ Left turn complete\n");
            
            delay(1000);
            
            Serial.println("========================================");
            Serial.println("TEST 3: Move Forward 20cm (after turn)");
            Serial.println("========================================");
            Serial.println("Starting forward motion...\n");
            
            test_state = 2;
            lastMoveTime = millis();
        }
    }
    
    // TEST 3: Move forward again
    else if (test_state == 2) {
        if (motion.moveForward(0.20)) {
            test3_done = true;
            Serial.println("\n✓ Forward movement complete\n");
            
            Serial.println("========================================");
            Serial.println("MOTION CONTROLLER TEST COMPLETE");
            Serial.println("========================================");
            Serial.println("✓ All tests passed!");
            Serial.println("  - Forward motion works");
            Serial.println("  - Turn works");
            Serial.println("  - Can move after turning");
            Serial.println("\n✓ Motion controller is ready!\n");
            
            test_state = 3;
        }
    }
    
    else {
        // Tests complete, just idle
        delay(1000);
    }
    
    delay(10);
}