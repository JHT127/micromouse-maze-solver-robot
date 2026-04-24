
#include "Encoder.h"


//------------------------------------------------------------
// GLOBAL POINTERS
// Purpose: ISR functions need access to encoder objects
//------------------------------------------------------------

Encoder* leftEncoderPtr = nullptr;
Encoder* rightEncoderPtr = nullptr;



//------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
// Purpose: Called by hardware when encoder ticks
//------------------------------------------------------------

void IRAM_ATTR leftEncoderISR() {

    if (leftEncoderPtr != nullptr) {
        leftEncoderPtr->handleInterrupt();
    }

}



void IRAM_ATTR rightEncoderISR() {

    if (rightEncoderPtr != nullptr) {
        rightEncoderPtr->handleInterrupt();
    }
    
}




//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Set up encoder with pin numbers
//------------------------------------------------------------

Encoder::Encoder(int pA, int pB, bool isLeft) {

    pinA = pA;
    pinB = pB;
    isLeftEncoder = isLeft;
    tickCount = 0;
    previousTickCount = 0;
    previousTime = 0;
    

    // Register this encoder globally for ISR
    if (isLeft) {
        leftEncoderPtr = this;
    } else {
        rightEncoderPtr = this;
    }

}




//------------------------------------------------------------
// INITIALIZE
// Purpose: Set up pins and attach interrupt
//------------------------------------------------------------

void Encoder::initialize() {

    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    

    // Attach interrupt to channel A
    if (isLeftEncoder) {
        attachInterrupt(digitalPinToInterrupt(pinA), leftEncoderISR, RISING);
    } else {
        attachInterrupt(digitalPinToInterrupt(pinA), rightEncoderISR, RISING);
    }
    
    reset();

}




//------------------------------------------------------------
// HANDLE INTERRUPT
// Purpose: Increment tick count when encoder changes
//------------------------------------------------------------

void IRAM_ATTR Encoder::handleInterrupt() {
    tickCount++;
}




//------------------------------------------------------------
// GET TICKS
// Purpose: Return total tick count
//------------------------------------------------------------

long Encoder::getTicks() {
    return tickCount;
}




//------------------------------------------------------------
// GET DISTANCE
// Purpose: Convert ticks to meters traveled
//------------------------------------------------------------

float Encoder::getDistance() {
    return tickCount * METERS_PER_TICK;
}




//------------------------------------------------------------
// GET VELOCITY
// Purpose: Calculate speed in meters per second
// Uses tick difference since last call
//------------------------------------------------------------

float Encoder::getVelocity(float deltaTime) {

    if (deltaTime <= 0) {
        return 0.0;
    }
    
    long currentTicks = tickCount;
    long deltaTicks = currentTicks - previousTickCount;
    
    float distance = deltaTicks * METERS_PER_TICK;
    float velocity = distance / deltaTime;
    
    previousTickCount = currentTicks;
    
    return velocity;

}



//------------------------------------------------------------
// RESET
// Purpose: Zero out all counters
//------------------------------------------------------------

void Encoder::reset() {
    tickCount = 0;
    previousTickCount = 0;
    previousTime = millis();
}

