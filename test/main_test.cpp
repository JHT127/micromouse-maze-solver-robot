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
// Purpose: Define all components for testing
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
LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);
FloodFill floodfill;
MazeNavigator mazeNavigator(&motion, &frontLidar, &leftLidar, &rightLidar, &floodfill);

//------------------------------------------------------------
// STATE SPACE EXAMPLE
// Purpose: Demonstrate position tracking functionality
//------------------------------------------------------------
void useStateSpaceExample() {
    robot.setStartPosition(1.0, 2.0, 0.5); // x, y, theta
    Serial.print("StateSpace: x="); Serial.print(robot.getX());
    Serial.print(", y="); Serial.print(robot.getY());
    Serial.print(", theta="); Serial.print(robot.getTheta());
    Serial.println();
    robot.printState();
}

//------------------------------------------------------------
// SETUP
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MAIN TEST");
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
    mazeNavigator.initialize(Cell(0, 0), NORTH);
    Serial.println("\u2713 All hardware ready\n");
    Serial.println("========================================");
    Serial.println("TEST: FloodFill Maze Navigation with StateSpace");
    Serial.println("========================================");
    useStateSpaceExample(); // Demonstrate StateSpace usage
}

//------------------------------------------------------------
// LOOP
//------------------------------------------------------------
void loop() {
    // Delegate navigation logic to MazeNavigator
    mazeNavigator.step();
    delay(20); // 50Hz control loop
}
