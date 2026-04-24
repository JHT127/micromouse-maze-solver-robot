
#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <Arduino.h>


//------------------------------------------------------------
// PID CONTROLLER CLASS
// Purpose: Adjust output to minimize error between target and actual
//------------------------------------------------------------


class PIDController {

private:

    //------------------------------------------------------------
    // PID GAINS
    // Purpose: Control how aggressively PID responds
    //------------------------------------------------------------

    float kp;  // Proportional gain (reacts to current error)
    float ki;  // Integral gain (reacts to accumulated error)
    float kd;  // Derivative gain (reacts to rate of change)
    


    //------------------------------------------------------------
    // ERROR TRACKING
    // Purpose: Remember previous values for I and D calculations
    //------------------------------------------------------------
    
    float previousError;
    float integral;
    


    //------------------------------------------------------------
    // OUTPUT LIMITS
    // Purpose: Prevent output from going too high or low
    //------------------------------------------------------------

    float outputMin;
    float outputMax;
    


    //------------------------------------------------------------
    // INTEGRAL LIMITS
    // Purpose: Prevent integral windup (accumulated error gets too big)
    //------------------------------------------------------------

    float integralMin;
    float integralMax;




public:

    //------------------------------------------------------------
    // SETUP
    // Purpose: Create PID controller with gains
    //------------------------------------------------------------

    PIDController();
    PIDController(float proportional, float integral, float derivative);
    


    //------------------------------------------------------------
    // CONFIGURATION
    // Purpose: Change PID behavior
    //------------------------------------------------------------
    
    void setGains(float proportional, float integral, float derivative);
    void setOutputLimits(float minOutput, float maxOutput);
    void setIntegralLimits(float minIntegral, float maxIntegral);
    


    //------------------------------------------------------------
    // MAIN FUNCTION
    // Purpose: Calculate control output based on error
    //------------------------------------------------------------

    float compute(float targetValue, float actualValue, float deltaTime);
    


    //------------------------------------------------------------
    // UTILITY
    // Purpose: Reset accumulated values
    //------------------------------------------------------------

    void reset();
    


    //------------------------------------------------------------
    // DEBUG
    // Purpose: See internal state
    //------------------------------------------------------------

    void printGains();
    float getIntegral();

    
};

#endif