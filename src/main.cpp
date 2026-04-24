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
#include "MazeNavigator.h"

//------------------------------------------------------------
// GLOBAL HARDWARE OBJECTS
// Purpose: Define all components
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

LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        leftPID, rightPID,
                        &imu, &robot);

FloodFill floodFill;

MazeNavigator navigator(&motion, &frontLidar, &leftLidar, &rightLidar, &floodFill);

//------------------------------------------------------------
// SETUP
// Purpose: Initialize all hardware and software
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MICROMOUSE MAZE SOLVER");
    Serial.println("========================================\n");
    
    //------------------------------------------------------------
    // INITIALIZE MOTORS AND ENCODERS
    //------------------------------------------------------------
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    robot.initialize();
    
    //------------------------------------------------------------
    // INITIALIZE IMU
    //------------------------------------------------------------
    Serial.println("Initializing IMU...");
    if (!imu.initialize()) {
        Serial.println("\u2717 MPU6050 not found!");
        while(1) delay(1000);
    }
    Serial.println("Calibrating IMU (keep still)...");
    imu.calibrate(500);
    
    //------------------------------------------------------------
    // INITIALIZE LIDAR SENSORS (SEQUENTIAL)
    //------------------------------------------------------------
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
    
    //------------------------------------------------------------
    // CONFIGURE PID CONTROLLERS
    //------------------------------------------------------------
    leftPID_forward.setOutputLimits(-255, 255);
    rightPID_forward.setOutputLimits(-255, 255);
    leftPID_turn.setOutputLimits(-255, 255);
    rightPID_turn.setOutputLimits(-255, 255);
    
    //------------------------------------------------------------
    // INITIALIZE MOTION CONTROLLER
    //------------------------------------------------------------
    motion.initialize();
    motion.setForwardSpeed(0.15);
    motion.setTurnSpeed(2.5);
    
    //------------------------------------------------------------
    // INITIALIZE FLOOD FILL AND NAVIGATOR
    //------------------------------------------------------------
    floodFill.initialize();
    navigator.initialize(Cell(0, 0), NORTH);
    Serial.println("\u2713 All hardware ready\n");
    Serial.println("========================================");
    Serial.println("TEST: FloodFill Maze Navigation with StateSpace");
    Serial.println("========================================");
}

//------------------------------------------------------------
// LOOP
// Purpose: Main control loop
//------------------------------------------------------------
void loop() {
    navigator.step();
    delay(20); // 50Hz control loop
}
