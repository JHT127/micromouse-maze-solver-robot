
#include "Motor.h"


//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Remember pin numbers when creating motor object
//------------------------------------------------------------

Motor::Motor(int pA, int pB, int pPWM) {
    pinA = pA;
    pinB = pB;
    pinPWM = pPWM;
    currentSpeed = 0;
}




//------------------------------------------------------------
// INITIALIZE
// Purpose: Set up pins as outputs and stop motor
//------------------------------------------------------------

void Motor::initialize() {
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinPWM, OUTPUT);
    
    stop();  // Start with motor stopped
}




//------------------------------------------------------------
// SET SPEED
// Purpose: Main control function - sets speed and direction
// Speed range: -255 (full backward) to +255 (full forward)
//------------------------------------------------------------

void Motor::setSpeed(int speed) {

    // Limit speed to valid range
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;
    
    currentSpeed = speed;
    

    if (speed > 0) {
        // FORWARD
        digitalWrite(pinA, LOW);
        digitalWrite(pinB, HIGH);
        analogWrite(pinPWM, speed);
        

    } else if (speed < 0) {
        // BACKWARD
        digitalWrite(pinA, HIGH);
        digitalWrite(pinB, LOW);
        analogWrite(pinPWM, -speed);  // Make positive for PWM
        

    } else {
        // STOP
        stop();
    }
    
}




//------------------------------------------------------------
// FORWARD
// Purpose: Spin forward at specified speed
//------------------------------------------------------------

void Motor::forward(int speed) {
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    
    setSpeed(speed);
}




//------------------------------------------------------------
// BACKWARD
// Purpose: Spin backward at specified speed
//------------------------------------------------------------

void Motor::backward(int speed) {
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    
    setSpeed(-speed);
}




//------------------------------------------------------------
// STOP
// Purpose: Let motor coast to a stop (no power)
//------------------------------------------------------------

void Motor::stop() {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    analogWrite(pinPWM, 0);
    currentSpeed = 0;
}




//------------------------------------------------------------
// BRAKE
// Purpose: Active braking (both pins HIGH stops motor quickly)
//------------------------------------------------------------

void Motor::brake() {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    analogWrite(pinPWM, 255);
    currentSpeed = 0;
}




//------------------------------------------------------------
// GET SPEED
// Purpose: Return current speed setting
//------------------------------------------------------------

int Motor::getSpeed() {
    return currentSpeed;
}




//------------------------------------------------------------
// IS MOVING
// Purpose: Check if motor has power
//------------------------------------------------------------

bool Motor::isMoving() {
    return currentSpeed != 0;
}

