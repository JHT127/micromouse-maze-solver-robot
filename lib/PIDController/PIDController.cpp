
#include "PIDController.h"


//------------------------------------------------------------
// CONSTRUCTOR (DEFAULT)
// Purpose: Create PID with zero gains
//------------------------------------------------------------

PIDController::PIDController() {
    kp = 0.0;
    ki = 0.0;
    kd = 0.0;
    
    previousError = 0.0;
    integral = 0.0;
    
    outputMin = -255.0;
    outputMax = 255.0;
    
    integralMin = -100.0;
    integralMax = 100.0;
}




//------------------------------------------------------------
// CONSTRUCTOR (WITH GAINS)
// Purpose: Create PID with specified gains
//------------------------------------------------------------

PIDController::PIDController(float proportional, float integralGain, float derivative) {
    kp = proportional;
    ki = integralGain;
    kd = derivative;
    
    previousError = 0.0;
    integral = 0.0;
    
    outputMin = -255.0;
    outputMax = 255.0;
    
    integralMin = -100.0;
    integralMax = 100.0;
}




//------------------------------------------------------------
// SET GAINS
// Purpose: Update PID gains (for tuning)
//------------------------------------------------------------

void PIDController::setGains(float proportional, float integralGain, float derivative) {
    kp = proportional;
    ki = integralGain;
    kd = derivative;
}




//------------------------------------------------------------
// SET OUTPUT LIMITS
// Purpose: Constrain output (e.g., motor PWM 0-255)
//------------------------------------------------------------

void PIDController::setOutputLimits(float minOutput, float maxOutput) {
    outputMin = minOutput;
    outputMax = maxOutput;
}




//------------------------------------------------------------
// SET INTEGRAL LIMITS
// Purpose: Prevent integral windup
//------------------------------------------------------------

void PIDController::setIntegralLimits(float minIntegral, float maxIntegral) {
    integralMin = minIntegral;
    integralMax = maxIntegral;
}




//------------------------------------------------------------
// COMPUTE
// Purpose: THE MAIN PID ALGORITHM
// Returns: Control output to apply to system
//------------------------------------------------------------

float PIDController::compute(float targetValue, float actualValue, float deltaTime) {
    // Prevent division by zero
    if (deltaTime <= 0.0) {
        return 0.0;
    }
    

    //------------------------------------------------------------
    // STEP 1: CALCULATE ERROR
    // Purpose: How far are we from target?
    //------------------------------------------------------------
    float error = targetValue - actualValue;
    


    //------------------------------------------------------------
    // STEP 2: PROPORTIONAL TERM
    // Purpose: React to current error
    // Higher error = stronger correction
    //------------------------------------------------------------
    float proportional = kp * error;
    


    //------------------------------------------------------------
    // STEP 3: INTEGRAL TERM
    // Purpose: React to accumulated error over time
    // Eliminates steady-state error
    //------------------------------------------------------------
    integral += error * deltaTime;
    


    // Prevent integral windup (limit accumulation)
    if (integral > integralMax) {
        integral = integralMax;
    }
    if (integral < integralMin) {
        integral = integralMin;
    }
    
    float integralTerm = ki * integral;
    



    //------------------------------------------------------------
    // STEP 4: DERIVATIVE TERM
    // Purpose: React to rate of change of error
    // Reduces overshoot and oscillation
    //------------------------------------------------------------
    float derivative = (error - previousError) / deltaTime;
    float derivativeTerm = kd * derivative;
    



    //------------------------------------------------------------
    // STEP 5: COMBINE ALL TERMS
    // Purpose: Total PID output
    //------------------------------------------------------------
    float output = proportional + integralTerm + derivativeTerm;
    


    //------------------------------------------------------------
    // STEP 6: LIMIT OUTPUT
    // Purpose: Keep output in valid range
    //------------------------------------------------------------
    if (output > outputMax) {
        output = outputMax;
    }
    if (output < outputMin) {
        output = outputMin;
    }
    


    //------------------------------------------------------------
    // STEP 7: REMEMBER FOR NEXT TIME
    // Purpose: Derivative needs previous error
    //------------------------------------------------------------
    previousError = error;
    


    return output;


}




//------------------------------------------------------------
// RESET
// Purpose: Clear all accumulated values
// Use when starting new control task
//------------------------------------------------------------

void PIDController::reset() {
    previousError = 0.0;
    integral = 0.0;
}




//------------------------------------------------------------
// PRINT GAINS
// Purpose: Debug - show current PID settings
//------------------------------------------------------------

void PIDController::printGains() {
    Serial.print("PID Gains: Kp=");
    Serial.print(kp, 4);
    Serial.print(", Ki=");
    Serial.print(ki, 4);
    Serial.print(", Kd=");
    Serial.println(kd, 4);
}



//------------------------------------------------------------
// GET INTEGRAL
// Purpose: Debug - check for integral windup
//------------------------------------------------------------

float PIDController::getIntegral() {
    return integral;
}

