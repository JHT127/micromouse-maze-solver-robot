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
#include "FloodFill.h"

//------------------------------------------------------------
// MOTION CONTROL
//------------------------------------------------------------
Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

PIDController leftPID_forward(500.0, 28.0, 6.0);
PIDController rightPID_forward(500.0, 28.0, 6.0);
PIDController leftPID_turn(65.0, 0.0, 0.0);
PIDController rightPID_turn(65.0, 0.0, 0.0);

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
// FLOODFILL MAZE
//------------------------------------------------------------
FloodFill floodfill;
// MAZE_SIZE is defined in FloodFill.h
Cell current(0, 0);
Direction heading = NORTH;

//------------------------------------------------------------
// SETUP
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("NEW FLOODFILL TEST");
    Serial.println("========================================\n");
    
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
    leftPID_forward.setOutputLimits(-255, 255);
    rightPID_forward.setOutputLimits(-255, 255);
    leftPID_turn.setOutputLimits(-255, 255);
    rightPID_turn.setOutputLimits(-255, 255);
    motion.initialize();
    motion.setForwardSpeed(0.15);
    motion.setTurnSpeed(2.5);
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
    frontLidar.startContinuous();
    leftLidar.startContinuous();
    rightLidar.startContinuous();
    delay(500);
    floodfill.initialize();
    Serial.println("\u2713 All hardware ready\n");
    Serial.println("========================================");
    Serial.println("TEST: FloodFill Maze Navigation");
    Serial.println("========================================");
}

//------------------------------------------------------------
// LOOP
//------------------------------------------------------------
void loop() {
    static unsigned long lastLidarRead = 0;
    static bool frontWall = false, leftWall = false, rightWall = false;
    static uint16_t frontDist = 0, leftDist = 0, rightDist = 0;
    static bool actionDone = true;
    static bool waitingForSensors = false;
    static unsigned long sensorWaitStart = 0;
    static bool readyToMove = false;
    unsigned long now = millis();

    // Step 1: Check (read sensors and update walls)
    if (actionDone && !waitingForSensors && !readyToMove && (now - lastLidarRead > 100)) {
        lastLidarRead = now;
        frontDist = frontLidar.readRangeContinuousMillimeters();
        leftDist = leftLidar.readRangeContinuousMillimeters();
        rightDist = rightLidar.readRangeContinuousMillimeters();
        frontWall = frontLidar.isWallDetected();
        leftWall = leftLidar.isWallDetected();
        rightWall = rightLidar.isWallDetected();
        Serial.print("FrontDist: "); Serial.print(frontDist);
        Serial.print(" LeftDist: "); Serial.print(leftDist);
        Serial.print(" RightDist: "); Serial.print(rightDist);
        Serial.print(" | Walls: F="); Serial.print(frontWall);
        Serial.print(" L="); Serial.print(leftWall);
        Serial.print(" R="); Serial.println(rightWall);
        floodfill.updateWalls(current, frontWall, rightWall, leftWall, heading);
        // Start non-blocking wait for sensors to stabilize
        motion.stop();
        waitingForSensors = true;
        sensorWaitStart = now;
        return;
    }

    // Step 2: Wait for sensors to stabilize (non-blocking)
    if (waitingForSensors) {
        if (now - sensorWaitStart >= 200) { // 200ms wait
            waitingForSensors = false;
            readyToMove = true; // Ready for decision and move
        } else {
            return; // Skip rest of loop until wait is done
        }
    }

    // Step 3: Decision and Move (cell by cell)
    if (readyToMove) {
        Direction nextMove = floodfill.getNextMove(current, heading);
        int turn = (nextMove - heading + 4) % 4;
        if (nextMove == heading && !frontWall) {
            // Move forward one cell
            motion.setPIDControllers(&leftPID_forward, &rightPID_forward);
            if (motion.moveForward(0.05)) {
                Serial.println("Moved forward 20cm (1 cell)");
                current = floodfill.getNeighbor(current, heading);
                Serial.print("Current cell: (");
                Serial.print(current.x);
                Serial.print(", ");
                Serial.print(current.y);
                Serial.println(")");
                actionDone = true;
                readyToMove = false;
            }
        } else if (turn == 1) { // Right turn
            motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
            if (motion.turnRight(PI / 2)) {
                Serial.println("Turned right");
                heading = (Direction)((heading + 1) % 4);
                actionDone = true;
                readyToMove = false;
            }
        } else if (turn == 3) { // Left turn
            motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
            if (motion.turnLeft(PI / 2)) {
                Serial.println("Turned left");
                heading = (Direction)((heading + 3) % 4);
                actionDone = true;
                readyToMove = false;
            }
        } else if (turn == 2) { // 180 turn
            motion.setPIDControllers(&leftPID_turn, &rightPID_turn);
            if (motion.turnRight(PI)) {
                Serial.println("Turned 180 degrees");
                heading = (Direction)((heading + 2) % 4);
                actionDone = true;
                readyToMove = false;
            }
        } else {
            Serial.println("No valid move found");
            actionDone = true;
            readyToMove = false;
        }
    }
    delay(20); // 50Hz control loop
}