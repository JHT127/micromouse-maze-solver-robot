
#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>


//------------------------------------------------------------
// MOTOR CLASS
// Purpose: Control one DC motor using L298N motor driver
//------------------------------------------------------------


class Motor {

private:

    //------------------------------------------------------------
    // PIN CONNECTIONS
    // Purpose: Remember which pins this motor uses
    //------------------------------------------------------------

    int pinA;       // Direction pin 1 (IN1 or IN3)
    int pinB;       // Direction pin 2 (IN2 or IN4)
    int pinPWM;     // Speed control pin (ENA or ENB)
    


    //------------------------------------------------------------
    // CURRENT STATE
    // Purpose: Track what motor is doing
    //------------------------------------------------------------

    int currentSpeed;  // -255 to +255 (negative = backward)




public:

    //------------------------------------------------------------
    // SETUP
    // Purpose: Tell motor which pins to use
    //------------------------------------------------------------

    Motor(int pA, int pB, int pPWM);
    void initialize();
    


    //------------------------------------------------------------
    // CONTROL FUNCTIONS
    // Purpose: Make motor do things
    //------------------------------------------------------------
    
    void setSpeed(int speed);        // -255 to +255
    void forward(int speed);         // 0 to 255
    void backward(int speed);        // 0 to 255
    void stop();
    void brake();
    


    //------------------------------------------------------------
    // STATUS
    // Purpose: Find out what motor is doing
    //------------------------------------------------------------

    int getSpeed();
    bool isMoving();

    
};


#endif