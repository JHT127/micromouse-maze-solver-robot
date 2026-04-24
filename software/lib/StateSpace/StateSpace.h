

#ifndef STATESPACE_H
#define STATESPACE_H

#include <Arduino.h>

//------------------------------------------------------------
// STATE SPACE MODEL
// Purpose: Track robot position using math from encoders/gyro
//------------------------------------------------------------


class StateSpace {
private:

    //------------------------------------------------------------
    // WHERE THE ROBOT IS
    // Purpose: The state vector [x, y, theta] 
    //------------------------------------------------------------

    float x;        // Position in meters (left/right)
    float y;        // Position in meters (forward/back)
    float theta;    // Heading angle in radians (0 = North)
    


    //------------------------------------------------------------
    // THE MATH MATRICES
    // Purpose: A and B matrices from linearization
    //------------------------------------------------------------

    float A[3][3];  // State transition matrix (how state changes naturally)
    float B[3][2];  // Control input matrix (how motors affect state)
    


    //------------------------------------------------------------
    // ROBOT MEASUREMENTS
    // Purpose: Physical properties of your actual robot
    //------------------------------------------------------------

    static constexpr float WHEELBASE = 0.08;  // 8cm between wheels
    


    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: Do the matrix math behind the scenes
    //------------------------------------------------------------

    void computeAMatrix();
    void computeBMatrix(float deltaTime);





public:

    //------------------------------------------------------------
    // SETUP
    // Purpose: Get everything ready to use
    //------------------------------------------------------------

    StateSpace();
    void initialize();
    void setStartPosition(float startX, float startY, float startAngle);
    



    //------------------------------------------------------------
    // MAIN PREDICTION FUNCTION
    // Purpose: Update position based on wheel movements
    //------------------------------------------------------------

    void predict(float forwardSpeed, float rotationSpeed, float deltaTime);
    



    //------------------------------------------------------------
    // READ CURRENT STATE
    // Purpose: Get the robot's current position
    //------------------------------------------------------------

    float getX();
    float getY();
    float getTheta();
    float getThetaDegrees();
    



    //------------------------------------------------------------
    // DEBUGGING TOOLS
    // Purpose: See what's happening inside
    //------------------------------------------------------------

    void printState();
    void printMatrices();
    void resetToOrigin();
};



#endif