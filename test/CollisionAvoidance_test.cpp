#include <Arduino.h>
#include <Wire.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "MPU6050Driver.h"
#include "StateSpace.h"
#include "MotionController.h"
#include "LIDARSensor.h"

//------------------------------------------------------------
// HARDWARE INSTANCES
//------------------------------------------------------------
Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

PIDController leftPID(4100.0, 40.0, 20.0);
PIDController rightPID(4100.0, 40.0, 20.0);

MPU6050Driver imu;
StateSpace stateSpace;
MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        &leftPID, &rightPID,
                        &imu, &stateSpace);

LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

//------------------------------------------------------------
// TEST STATE
//------------------------------------------------------------
bool testRunning = false;
unsigned long testStartTime = 0;
unsigned long collisionTime = 0;
bool collisionDetected = false;

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("\n\n========================================");
    Serial.println("COLLISION AVOIDANCE TEST");
    Serial.println("========================================\n");
    
    //------------------------------------------------------------
    // STEP 1: I2C INIT
    //------------------------------------------------------------
    Serial.println("Step 1: Initializing I2C...");
    Wire.begin();
    delay(100);
    
    //------------------------------------------------------------
    // STEP 2: MOTOR INIT
    //------------------------------------------------------------
    Serial.println("Step 2: Initializing motors...");
    leftMotor.initialize();
    rightMotor.initialize();
    leftMotor.setSpeed(0);
    rightMotor.setSpeed(0);
    delay(500);
    
    //------------------------------------------------------------
    // STEP 3: ENCODER INIT
    //------------------------------------------------------------
    Serial.println("Step 3: Initializing encoders...");
    leftEncoder.initialize();
    rightEncoder.initialize();
    leftEncoder.reset();
    rightEncoder.reset();
    delay(100);
    
    //------------------------------------------------------------
    // STEP 4: IMU INIT
    //------------------------------------------------------------
    Serial.println("Step 4: Initializing IMU...");
    if (!imu.initialize()) {
        Serial.println("  IMU FAILED!");
        while(1);
    }
    imu.calibrate(100);  // 100 samples for calibration
    delay(100);
    
    //------------------------------------------------------------
    // STEP 5: LIDAR INIT
    //------------------------------------------------------------
    Serial.println("Step 5: Initializing LIDARs...");
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(100);
    
    Serial.println("  Enabling FRONT...");
    frontLidar.enable();
    delay(50);
    if (!frontLidar.initialize(0x30)) {
        Serial.println("    FRONT initialization FAILED!");
    }
    
    Serial.println("  Enabling LEFT...");
    leftLidar.enable();
    delay(50);
    if (!leftLidar.initialize(0x31)) {
        Serial.println("    LEFT initialization FAILED!");
    }
    
    Serial.println("  Enabling RIGHT...");
    rightLidar.enable();
    delay(50);
    if (!rightLidar.initialize(0x32)) {
        Serial.println("    RIGHT initialization FAILED!");
    }
    
    //------------------------------------------------------------
    // STEP 6: MOTION CONTROLLER INIT
    //------------------------------------------------------------
    Serial.println("Step 6: Initializing Motion Controller...");
    stateSpace.initialize();
    leftPID.setOutputLimits(0, 255);
    rightPID.setOutputLimits(0, 255);
    motion.initialize();
    motion.setForwardSpeed(0.15);  // 0.15 m/s = low speed for safety
    
    //------------------------------------------------------------
    // STEP 7: START CONTINUOUS LIDAR MODE
    //------------------------------------------------------------
    Serial.println("Step 7: Starting continuous LIDAR mode...");
    frontLidar.startContinuous();
    leftLidar.startContinuous();
    rightLidar.startContinuous();
    delay(500);  // Let sensors stabilize
    
    //------------------------------------------------------------
    // STEP 8: DISPLAY CONFIG
    //------------------------------------------------------------
    Serial.println("\n========================================");
    Serial.println("TEST CONFIGURATION");
    Serial.println("========================================");
    Serial.println("Forward Speed: 0.15 m/s");
    Serial.println("Wall Detection Threshold: 200mm (20cm)");
    Serial.println("Test: Move forward until wall detected");
    Serial.println("\nStarting in 3 seconds...\n");
    delay(3000);
    
    //------------------------------------------------------------
    // START MOTION
    //------------------------------------------------------------
    Serial.println("Starting forward motion...");
    testRunning = true;
    testStartTime = millis();
    collisionDetected = false;
    motion.moveForward(5.0);  // Try to move 5 meters forward
}

void loop() {
    if (!testRunning) {
        delay(1000);
        return;
    }
    
    unsigned long elapsed = millis() - testStartTime;
    
    //------------------------------------------------------------
    // CONTINUOUS MOTION CONTROL (MUST CALL EVERY LOOP)
    //------------------------------------------------------------
    motion.moveForward(5.0);  // Keep calling to update PID control
    
    //------------------------------------------------------------
    // CHECK WALLS
    //------------------------------------------------------------
    bool frontWall = frontLidar.isWallDetected();
    bool leftWall = leftLidar.isWallDetected();
    bool rightWall = rightLidar.isWallDetected();
    
    uint16_t frontDist = frontLidar.readRangeContinuousMillimeters();
    uint16_t leftDist = leftLidar.readRangeContinuousMillimeters();
    uint16_t rightDist = rightLidar.readRangeContinuousMillimeters();
    
    //------------------------------------------------------------
    // COLLISION DETECTION (FRONT WALL ONLY)
    //------------------------------------------------------------
    if (frontWall && !collisionDetected) {
        collisionDetected = true;
        collisionTime = elapsed;
        motion.stop();
        
        Serial.println("\n\n========================================");
        Serial.println("COLLISION DETECTED!");
        Serial.println("========================================");
    }
    
    //------------------------------------------------------------
    // PRINT STATUS EVERY 200ms
    //------------------------------------------------------------
    if (elapsed % 200 < 50) {
        unsigned long secs = elapsed / 1000;
        unsigned long ms = elapsed % 1000;
        char timeStr[16];
        sprintf(timeStr, "%lu.%03lu", secs, ms);
        
        Serial.print(timeStr);
        Serial.print("s: [");
        Serial.print(frontWall ? "WALL" : "OK  ");
        Serial.print("] [");
        Serial.print(leftWall ? "WALL" : "OK  ");
        Serial.print("] [");
        Serial.print(rightWall ? "WALL" : "OK  ");
        Serial.print("]  (");
        Serial.print(frontDist);
        Serial.print("mm, ");
        Serial.print(leftDist);
        Serial.print("mm, ");
        Serial.print(rightDist);
        Serial.println("mm)");
    }
    
    //------------------------------------------------------------
    // STOP TEST AFTER COLLISION OR 60 SECONDS
    //------------------------------------------------------------
    if (collisionDetected || elapsed > 60000) {
        testRunning = false;
        
        if (!collisionDetected) {
            Serial.println("\n========================================");
            Serial.println("TEST TIMEOUT - NO COLLISION");
            Serial.println("========================================");
        }
        
        Serial.print("\nTest Duration: ");
        Serial.print(elapsed / 1000);
        Serial.println(" seconds");
        
        if (collisionDetected) {
            Serial.print("Collision at: ");
            Serial.print(collisionTime / 1000);
            Serial.println(" seconds");
        }
        
        Serial.println("\nTest Complete!");
        
        // Stop motors
        leftMotor.setSpeed(0);
        rightMotor.setSpeed(0);
        
        // Stop LIDARs
        frontLidar.stopContinuous();
        leftLidar.stopContinuous();
        rightLidar.stopContinuous();
    }
    
    delay(50);
}
