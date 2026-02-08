
#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>


//------------------------------------------------------------
// ENCODER CLASS
// Purpose: Read wheel rotation from magnetic encoder
//------------------------------------------------------------


class Encoder {

private:

    //------------------------------------------------------------
    // PIN CONNECTIONS
    // Purpose: Remember which pins this encoder uses
    //------------------------------------------------------------

    int pinA;       // Channel A (main counting pin)
    int pinB;       // Channel B (direction detection, not used yet)
    


    //------------------------------------------------------------
    // ENCODER PROPERTIES
    // Purpose: Physical characteristics of N20 motor encoder
    //------------------------------------------------------------

    static const int TICKS_PER_REVOLUTION = 530;
    static constexpr float WHEEL_DIAMETER = 0.033;  // 33mm in meters
    static constexpr float WHEEL_RADIUS = WHEEL_DIAMETER / 2.0;
    static constexpr float WHEEL_CIRCUMFERENCE = PI * WHEEL_DIAMETER;
    static constexpr float METERS_PER_TICK = WHEEL_CIRCUMFERENCE / TICKS_PER_REVOLUTION;
    


    //------------------------------------------------------------
    // TICK COUNTING
    // Purpose: Track wheel rotations
    //------------------------------------------------------------

    volatile long tickCount;
    long previousTickCount;
    unsigned long previousTime;
    



    //------------------------------------------------------------
    // IDENTIFICATION
    // Purpose: Know if this is left or right encoder
    //------------------------------------------------------------

    bool isLeftEncoder;




public:

    //------------------------------------------------------------
    // SETUP
    // Purpose: Configure encoder pins and attach interrupt
    //------------------------------------------------------------

    Encoder(int pA, int pB, bool isLeft);
    void initialize();
    



    //------------------------------------------------------------
    // READING FUNCTIONS
    // Purpose: Get encoder data
    //------------------------------------------------------------

    long getTicks();
    float getDistance();                    // Total distance in meters
    float getVelocity(float deltaTime);     // Speed in m/s
    



    //------------------------------------------------------------
    // UTILITY
    // Purpose: Reset and manage encoder
    //------------------------------------------------------------

    void reset();
    



    //------------------------------------------------------------
    // INTERRUPT HANDLER
    // Purpose: Count ticks when encoder pin changes
    //------------------------------------------------------------

    void IRAM_ATTR handleInterrupt();

};




//------------------------------------------------------------
// GLOBAL ENCODER INSTANCES
// Purpose: Need global pointers for interrupt handlers
//------------------------------------------------------------

extern Encoder* leftEncoderPtr;
extern Encoder* rightEncoderPtr;




//------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
// Purpose: Called by hardware when encoder pins change
//------------------------------------------------------------

void IRAM_ATTR leftEncoderISR();
void IRAM_ATTR rightEncoderISR();




#endif