#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include <Arduino.h>
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "MPU6050Driver.h"
#include "StateSpace.h"

//------------------------------------------------------------
// MOTION CONTROLLER CLASS
// Purpose: High-level movement commands
//------------------------------------------------------------

class MotionController {
private:
    //------------------------------------------------------------
    // HARDWARE REFERENCES
    // Purpose: Access to all low-level components
    //------------------------------------------------------------
    Motor* leftMotor;
    Motor* rightMotor;
    Encoder* leftEncoder;
    Encoder* rightEncoder;
    PIDController* leftPID;
    PIDController* rightPID;
    MPU6050Driver* imu;
    StateSpace* stateSpace;
    
    //------------------------------------------------------------
    // CONTROL PARAMETERS
    // Purpose: Tunable motion settings
    //------------------------------------------------------------
    float forwardSpeed;         // Target speed for straight motion (m/s)
    float turnSpeed;            // Target angular velocity for turns (rad/s)
    static constexpr float UPDATE_RATE = 0.02;  // 50Hz control loop
    
    //------------------------------------------------------------
    // TOLERANCES
    // Purpose: Acceptable error margins
    //------------------------------------------------------------
    static constexpr float DISTANCE_TOLERANCE = 0.005;  // 5mm
    static constexpr float ANGLE_TOLERANCE = 0.0349;    // 2 degrees in radians
    
    //------------------------------------------------------------
    // STATE TRACKING
    // Purpose: Monitor motion completion
    //------------------------------------------------------------
    unsigned long lastUpdateTime;
    bool isMovingFlag;
    
    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: Internal control logic
    //------------------------------------------------------------
    void updateStateSpace(float dt);
    bool isDistanceReached(float target, float current);
    bool isAngleReached(float target, float current);

public:
        // Set PID controllers at runtime
        void setPIDControllers(PIDController* lpid, PIDController* rpid);
    //------------------------------------------------------------
    // CONSTRUCTOR
    // Purpose: Link to hardware components
    //------------------------------------------------------------
    MotionController(Motor* lm, Motor* rm, 
                     Encoder* le, Encoder* re,
                     PIDController* lpid, PIDController* rpid,
                     MPU6050Driver* imu_ptr,
                     StateSpace* ss);
    
    //------------------------------------------------------------
    // CONFIGURATION
    // Purpose: Set motion parameters
    //------------------------------------------------------------
    void initialize();
    void setForwardSpeed(float speed);  // Default 0.15 m/s
    void setTurnSpeed(float speed);     // Default 1.57 rad/s (90°/s)
    
    //------------------------------------------------------------
    // MOTION PRIMITIVES
    // Purpose: Main movement commands
    //------------------------------------------------------------
    bool moveForward(float distance);   // Move straight, returns true when done
    bool moveBackward(float distance);  // Reverse
    bool turnLeft(float angle);         // Rotate counterclockwise (radians)
    bool turnRight(float angle);        // Rotate clockwise (radians)
    void stop();                        // Emergency stop
    
    //------------------------------------------------------------
    // STATUS
    // Purpose: Query motion state
    //------------------------------------------------------------
    bool isMoving();
};

#endif
