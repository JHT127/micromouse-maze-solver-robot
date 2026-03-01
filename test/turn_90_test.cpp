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

PIDController leftPID(65.0, 0.0, 0.0);
PIDController rightPID(65.0, 0.0, 0.0);

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
    Serial.begin(115200);
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
    leftPID.setOutputLimits(-255, 255);
    rightPID.setOutputLimits(-255, 255);
    
    motion.initialize();
    motion.setForwardSpeed(0.15);  // m/s
    motion.setTurnSpeed(2.5);     // rad/s
    
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
    
    

    // TEST 1: TURN RIGHT 90 DEGREES
    if (test_state == 0) {
        if (motion.turnRight(PI / 2)) {
            Serial.println("✓ Turn Right 90° complete");
            delay(1500); // Pause between turns
            test_state = 1;
            Serial.println("Starting Test 2: Turn Left 90°...");
        }
    }
    
    // TEST 2: TURN LEFT 90 DEGREES
    else if (test_state == 1) {
        if (motion.turnLeft(PI / 2)) {
            Serial.println("✓ Turn Left 90° complete");
            Serial.println("\n=== ALL TURNS COMPLETE ===");
            test_state = 2;
        }
    }
    
    // Debug output every 250ms
    if (test_state < 2 && (now % 250 < 20)) {
        Serial.print("Current Angle: ");
        // Negate to show the standard convention (CCW positive)
        Serial.print(-imu.getAngleDegrees(), 1);
        Serial.println("°");
    }
    
    delay(10);
}