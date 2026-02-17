#include "MotionController.h"

//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Store hardware references
//------------------------------------------------------------
MotionController::MotionController(Motor* lm, Motor* rm, 
                                   Encoder* le, Encoder* re,
                                   PIDController* lpid, PIDController* rpid,
                                   MPU6050Driver* imu_ptr,
                                   StateSpace* ss)
    : leftMotor(lm), rightMotor(rm),
      leftEncoder(le), rightEncoder(re),
      leftPID(lpid), rightPID(rpid),
      imu(imu_ptr), stateSpace(ss)
{
    forwardSpeed = 0.15;
    turnSpeed = 1.57;
    lastUpdateTime = millis();
    isMovingFlag = false;
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Reset all components
//------------------------------------------------------------
void MotionController::initialize() {
    leftEncoder->reset();
    rightEncoder->reset();
    imu->resetAngle();
    stateSpace->resetToOrigin();
    
    leftPID->reset();
    rightPID->reset();
    
    leftMotor->stop();
    rightMotor->stop();
    
    isMovingFlag = false;
    lastUpdateTime = millis();
    
    Serial.println("MotionController initialized");
}

//------------------------------------------------------------
// SET FORWARD SPEED
// Purpose: Configure target speed for straight motion
//------------------------------------------------------------
void MotionController::setForwardSpeed(float speed) {
    forwardSpeed = speed;
}

//------------------------------------------------------------
// SET TURN SPEED
// Purpose: Configure target angular velocity for turns
//------------------------------------------------------------
void MotionController::setTurnSpeed(float speed) {
    turnSpeed = speed;
}

//------------------------------------------------------------
// MOVE FORWARD
// Purpose: Drive straight for specified distance
//------------------------------------------------------------
bool MotionController::moveForward(float distance) {
    if (!isMovingFlag) {
        // Start motion
        leftEncoder->reset();
        rightEncoder->reset();
        imu->resetAngle();
        stateSpace->setState(0, 0, 0);
        isMovingFlag = true;
        lastUpdateTime = millis();
        
        Serial.print("Moving forward: ");
        Serial.print(distance);
        Serial.println(" m");
    }
    
    float dt = UPDATE_RATE;
    
    // Get velocities from encoders
    float leftVel = leftEncoder->getVelocity(dt);
    float rightVel = rightEncoder->getVelocity(dt);
    float avgVel = (leftVel + rightVel) / 2.0;
    
    // Compute PID outputs
    float leftPWM = leftPID->compute(forwardSpeed, leftVel, dt);
    float rightPWM = rightPID->compute(forwardSpeed, rightVel, dt);
    
    // Set motor speeds
    leftMotor->setSpeed((int)leftPWM);
    rightMotor->setSpeed((int)rightPWM);
    
    // Update state space
    float omega = (rightVel - leftVel) / 0.08;  // WHEELBASE = 0.08m
    stateSpace->predict(avgVel, omega, dt);
    imu->update(dt);
    
    // Check if target distance reached
    float currentDistance = leftEncoder->getDistance();
    if (isDistanceReached(distance, currentDistance)) {
        stop();
        isMovingFlag = false;
        
        Serial.print("Movement complete. Distance: ");
        Serial.print(currentDistance);
        Serial.println(" m");
        
        return true;
    }
    
    delay(UPDATE_RATE * 1000);
    return false;
}

//------------------------------------------------------------
// MOVE BACKWARD
// Purpose: Drive backward for specified distance
//------------------------------------------------------------
bool MotionController::moveBackward(float distance) {
    if (!isMovingFlag) {
        leftEncoder->reset();
        rightEncoder->reset();
        imu->resetAngle();
        stateSpace->setState(0, 0, 0);
        isMovingFlag = true;
        lastUpdateTime = millis();
        
        Serial.print("Moving backward: ");
        Serial.print(distance);
        Serial.println(" m");
    }
    
    float dt = UPDATE_RATE;
    
    float leftVel = leftEncoder->getVelocity(dt);
    float rightVel = rightEncoder->getVelocity(dt);
    float avgVel = (leftVel + rightVel) / 2.0;
    
    float targetSpeed = -forwardSpeed;
    float leftPWM = leftPID->compute(targetSpeed, leftVel, dt);
    float rightPWM = rightPID->compute(targetSpeed, rightVel, dt);
    
    leftMotor->setSpeed((int)leftPWM);
    rightMotor->setSpeed((int)rightPWM);
    
    float omega = (rightVel - leftVel) / 0.08;
    stateSpace->predict(avgVel, omega, dt);
    imu->update(dt);
    
    float currentDistance = leftEncoder->getDistance();
    if (isDistanceReached(distance, currentDistance)) {
        stop();
        isMovingFlag = false;
        
        Serial.print("Backward movement complete. Distance: ");
        Serial.print(currentDistance);
        Serial.println(" m");
        
        return true;
    }
    
    delay(UPDATE_RATE * 1000);
    return false;
}

//------------------------------------------------------------
// TURN LEFT
// Purpose: Rotate counterclockwise by specified angle
//------------------------------------------------------------
bool MotionController::turnLeft(float angle) {
    if (!isMovingFlag) {
        imu->resetAngle();
        stateSpace->setState(0, 0, 0);
        isMovingFlag = true;
        lastUpdateTime = millis();
        
        Serial.print("Turning left: ");
        Serial.print(angle * 180 / PI);
        Serial.println(" degrees");
    }
    
    float dt = UPDATE_RATE;
    
    // Left wheel backward, right wheel forward for counterclockwise
    float angularVel = imu->getAngularVelocity();
    
    // PID to control angular velocity
    float speedCommand = rightPID->compute(turnSpeed, angularVel, dt);
    
    leftMotor->setSpeed(-(int)speedCommand);
    rightMotor->setSpeed((int)speedCommand);
    
    imu->update(dt);
    
    float currentAngle = imu->getAngle();
    if (isAngleReached(angle, currentAngle)) {
        stop();
        isMovingFlag = false;
        
        Serial.print("Left turn complete. Angle: ");
        Serial.print(currentAngle * 180 / PI);
        Serial.println(" degrees");
        
        return true;
    }
    
    delay(UPDATE_RATE * 1000);
    return false;
}

//------------------------------------------------------------
// TURN RIGHT
// Purpose: Rotate clockwise by specified angle
//------------------------------------------------------------
bool MotionController::turnRight(float angle) {
    if (!isMovingFlag) {
        imu->resetAngle();
        stateSpace->setState(0, 0, 0);
        isMovingFlag = true;
        lastUpdateTime = millis();
        
        Serial.print("Turning right: ");
        Serial.print(angle * 180 / PI);
        Serial.println(" degrees");
    }
    
    float dt = UPDATE_RATE;
    
    // Right wheel backward, left wheel forward for clockwise
    float angularVel = imu->getAngularVelocity();
    
    float speedCommand = rightPID->compute(turnSpeed, -angularVel, dt);
    
    leftMotor->setSpeed((int)speedCommand);
    rightMotor->setSpeed(-(int)speedCommand);
    
    imu->update(dt);
    
    float currentAngle = imu->getAngle();
    if (isAngleReached(angle, -currentAngle)) {
        stop();
        isMovingFlag = false;
        
        Serial.print("Right turn complete. Angle: ");
        Serial.print(currentAngle * 180 / PI);
        Serial.println(" degrees");
        
        return true;
    }
    
    delay(UPDATE_RATE * 1000);
    return false;
}

//------------------------------------------------------------
// STOP
// Purpose: Emergency stop
//------------------------------------------------------------
void MotionController::stop() {
    leftMotor->stop();
    rightMotor->stop();
    isMovingFlag = false;
}

//------------------------------------------------------------
// IS MOVING
// Purpose: Check if in motion
//------------------------------------------------------------
bool MotionController::isMoving() {
    return isMovingFlag;
}

//------------------------------------------------------------
// IS DISTANCE REACHED
// Purpose: Check if target distance achieved
//------------------------------------------------------------
bool MotionController::isDistanceReached(float target, float current) {
    float error = abs(target - current);
    return error < DISTANCE_TOLERANCE;
}

//------------------------------------------------------------
// IS ANGLE REACHED
// Purpose: Check if target angle achieved
//------------------------------------------------------------
bool MotionController::isAngleReached(float target, float current) {
    float error = abs(target - current);
    return error < ANGLE_TOLERANCE;
}

//------------------------------------------------------------
// UPDATE STATE SPACE
// Purpose: Integrate motion into state space
//------------------------------------------------------------
void MotionController::updateStateSpace(float dt) {
    float leftVel = leftEncoder->getVelocity(dt);
    float rightVel = rightEncoder->getVelocity(dt);
    
    float v = (leftVel + rightVel) / 2.0;
    float omega = (rightVel - leftVel) / 0.08;
    
    stateSpace->predict(v, omega, dt);
}
