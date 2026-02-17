
#include <Arduino.h>
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

PIDController leftPID(500.0, 28.0, 6.0);
PIDController rightPID(500.0, 28.0, 6.0);

StateSpace robot;

MPU6050Driver imu;

LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        &leftPID, &rightPID,
                        &imu, &robot);

FloodFill floodFill;

MazeNavigator navigator(&motion, &frontLidar, &leftLidar, &rightLidar, &floodFill);

//------------------------------------------------------------
// SETUP
// Purpose: Initialize all hardware and software
//------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== MAZE NAVIGATOR INTEGRATION TEST ===");
    Serial.println("Initializing...\n");
    
    // Initialize motors and encoders
    Serial.println("Initializing motors...");
    leftMotor.initialize();
    rightMotor.initialize();
    
    Serial.println("Initializing encoders...");
    leftEncoder.initialize();
    rightEncoder.initialize();
    
    Serial.println("Initializing state space...");
    robot.initialize();
    
    delay(100);
    
    // Initialize IMU
    Serial.println("Initializing MPU6050...");
    if (!imu.initialize()) {
        Serial.println("ERROR: MPU6050 initialization failed!");
        while(1) delay(100);
    }
    
    Serial.println("Calibrating MPU6050 (keep robot still)...");
    delay(1000);
    if (!imu.calibrate(500)) {
        Serial.println("ERROR: MPU6050 calibration failed!");
        while(1) delay(100);
    }
    
    delay(500);
    
    // Initialize LIDAR sensors (sequential)
    Serial.println("Disabling all LIDAR sensors...");
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(50);
    
    Serial.println("Initializing front LIDAR...");
    frontLidar.enable();
    delay(50);
    if (!frontLidar.initialize(0x30)) {
        Serial.println("ERROR: Front LIDAR failed!");
        while(1) delay(100);
    }
    
    Serial.println("Initializing left LIDAR...");
    leftLidar.enable();
    delay(50);
    if (!leftLidar.initialize(0x31)) {
        Serial.println("ERROR: Left LIDAR failed!");
        while(1) delay(100);
    }
    
    Serial.println("Initializing right LIDAR...");
    rightLidar.enable();
    delay(50);
    if (!rightLidar.initialize(0x32)) {
        Serial.println("ERROR: Right LIDAR failed!");
        while(1) delay(100);
    }
    
    delay(500);
    
    // Configure PID
    Serial.println("Configuring PID controllers...");
    leftPID.setOutputLimits(0, 255);
    rightPID.setOutputLimits(0, 255);
    
    // Initialize motion controller
    Serial.println("Initializing motion controller...");
    motion.initialize();
    motion.setForwardSpeed(0.15);
    motion.setTurnSpeed(1.57);
    
    // Initialize maze
    Serial.println("Initializing maze...");
    floodFill.initialize();
    
    Serial.println("\n=== INITIALIZATION COMPLETE ===");
    Serial.println("Starting maze exploration...\n");
    
    delay(1000);
    
    // Start exploration
    navigator.initialize(Cell(0, 0), NORTH);
    navigator.exploreMode();
    
    Serial.println("\nExploration finished!");
}


//------------------------------------------------------------
// LOOP
// Purpose: Just idle after exploration
//------------------------------------------------------------

void loop() {
    delay(10000);
}
