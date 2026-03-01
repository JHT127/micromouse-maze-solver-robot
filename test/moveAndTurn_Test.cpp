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

// Use PID values from MoveForward_WallDetection_test.cpp for forward, but you can adjust as needed

// Forward PID controllers
PIDController leftPID_forward(500.0, 28.0, 6.0);
PIDController rightPID_forward(500.0, 28.0, 6.0);
// Turn PID controllers
PIDController leftPID_turn(65.0, 0.0, 0.0);
PIDController rightPID_turn(65.0, 0.0, 0.0);

// Pointer to current active PIDs
PIDController* leftPID = &leftPID_forward;
PIDController* rightPID = &rightPID_forward;

StateSpace robot;
MPU6050Driver imu;

MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        leftPID, rightPID,
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
bool test1_done = false;
bool test2_done = false;
bool turn_done = false;
int test_state = 1; // 1: move forward until wall, 2: turn right 90 deg

//------------------------------------------------------------
// SETUP
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MOVE FORWARD + TURN 90 TEST");
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
        Serial.println("\u2717 MPU6050 not found!");
        while(1) delay(1000);
    }
    Serial.println("Calibrating IMU (keep still)...");
    imu.calibrate(500);
    Serial.println("Configuring PID...");
    leftPID_forward.setOutputLimits(-255, 255);
    rightPID_forward.setOutputLimits(-255, 255);
    leftPID_turn.setOutputLimits(-255, 255);
    rightPID_turn.setOutputLimits(-255, 255);
    motion.initialize();
    motion.setForwardSpeed(0.15);  // m/s
    motion.setTurnSpeed(2.5);     // rad/s
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
    Serial.println("\u2713 All hardware ready\n");
    Serial.println("========================================");
    Serial.println("TEST: Move Forward, then Turn 90");
    Serial.println("========================================");
    Serial.println("Starting forward motion...\n");
    lastMoveTime = millis();
}

//------------------------------------------------------------
// LOOP
//------------------------------------------------------------
void loop() {
    unsigned long now = millis();
    static bool frontWall = false;
    static uint16_t frontDist = 0;
    static unsigned long lastLidarRead = 0;

    if (test_state == 1) {
        // Ensure forward PIDs are active
        motion.setPIDControllers(&leftPID_forward, &rightPID_forward);
        // Move forward until wall detected
        if (now - lastLidarRead > 100) {
            lastLidarRead = now;
            frontDist = frontLidar.readRangeContinuousMillimeters();
            frontWall = frontLidar.isWallDetected();
            if (frontWall) {
                motion.stop();
                Serial.println("\n\n\u2717 WALL DETECTED!");
                Serial.print("Front Distance: ");
                Serial.print(frontDist);
                Serial.println("mm");
                Serial.println("Motion stopped!\n");
                // Switch to turn PIDs
                motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
                test_state = 2; // Go to turn
                delay(500); // Short pause before turning
            }
        }
        // Try to move forward (runs every 20ms)
        if (!frontWall && !motion.moveForward(0.90)) {
            // Still moving
        } else if (frontWall) {
            // Wall detected, handled above
        } else {
            // Movement complete (not expected in this test)
            test1_done = true;
            Serial.println("\n\u2713 Forward movement complete\n");
            // Switch to turn PIDs
            motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
            test_state = 2;
        }
    }
    else if (test_state == 2) {
        // Ensure turn PIDs are active
        motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
        // Turn right 90 degrees (PI/2) with turn PID
        if (motion.turnRight(PI / 2)) {
            Serial.println("\u2713 Turn Right 90\u00b0 complete");
            turn_done = true;
            Serial.println("\n=== MOVE + TURN COMPLETE ===");
            test_state = 3;
        }
        // Optionally, print angle for debug
        if (!turn_done && (now % 250 < 20)) {
            Serial.print("Current Angle: ");
            Serial.print(-imu.getAngleDegrees(), 1);
            Serial.println("\u00b0");
        }
    }
    else {
        // Tests complete, just idle
        delay(1000);
    }
    delay(20);  // 50Hz control loop
}
