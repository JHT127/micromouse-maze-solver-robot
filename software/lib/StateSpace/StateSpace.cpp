
#include "StateSpace.h"
#include <math.h>



//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Initialize everything to zero when object is created
//------------------------------------------------------------

StateSpace::StateSpace() {
    x = 0.0;
    y = 0.0;
    theta = 0.0;
    
    // Zero out all matrix values
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            A[i][j] = 0.0;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            B[i][j] = 0.0;
        }
    }
}




//------------------------------------------------------------
// INITIALIZE
// Purpose: Set up the A matrix (it never changes)
//------------------------------------------------------------

void StateSpace::initialize() {
    resetToOrigin();
    computeAMatrix();
}




//------------------------------------------------------------
// SET START POSITION
// Purpose: Start from any position (not just 0,0)
//------------------------------------------------------------

void StateSpace::setStartPosition(float startX, float startY, float startAngle) {
    x = startX;
    y = startY;
    theta = startAngle;
}




//------------------------------------------------------------
// RESET TO ORIGIN
// Purpose: Go back to (0, 0, 0°) - useful for testing
//------------------------------------------------------------

void StateSpace::resetToOrigin() {
    x = 0.0;
    y = 0.0;
    theta = 0.0;
}




//------------------------------------------------------------
// COMPUTE A MATRIX
// Purpose: Create the state transition matrix (identity matrix)
// Why identity: Robot doesn't move without control input
//------------------------------------------------------------

void StateSpace::computeAMatrix() {

    // A is identity because robot at rest stays at rest
    // Partial derivatives of state with respect to state (at v=0, omega=0)
    

    // Clear everything
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            A[i][j] = 0.0;
        }
    }
    

    // Set diagonal to 1
    A[0][0] = 1.0;  // x stays x
    A[1][1] = 1.0;  // y stays y
    A[2][2] = 1.0;  // theta stays theta

}




//------------------------------------------------------------
// COMPUTE B MATRIX
// Purpose: Calculate how control inputs affect position
// This is the LINEARIZATION 
//------------------------------------------------------------

void StateSpace::computeBMatrix(float deltaTime) {
    
    //Partial derivatives of state with respect to control input
    // This is the Jacobian that linearizes the nonlinear dynamics
    

    // How forward velocity affects X position

    B[0][0] = sin(theta) * deltaTime;   // dx/dv (using North=0 coordinate system)
    B[0][1] = 0.0;                       // dx/domega
    

    // How forward velocity affects Y position

    B[1][0] = cos(theta) * deltaTime;   // dy/dv
    B[1][1] = 0.0;                       // dy/domega
    

    // How rotation affects heading angle

    B[2][0] = 0.0;                       // dtheta/dv
    B[2][1] = deltaTime;                 // dtheta/domega

}




//------------------------------------------------------------
// PREDICT
// Purpose: THE MAIN FUNCTION - updates robot position
// This implements: x_new = A*x_old + B*u 
//------------------------------------------------------------

void StateSpace::predict(float forwardSpeed, float rotationSpeed, float deltaTime) {

    // STEP 1: Linearize around current angle
    // B matrix depends on current theta, so recompute it
    computeBMatrix(deltaTime);
    

    // STEP 2: Save old state
    float xOld = x;
    float yOld = y;
    float thetaOld = theta;
    

    // STEP 3: Matrix multiplication A * x_old
    // Since A is identity matrix, this just gives us the old state back
    float Ax[3];
    Ax[0] = A[0][0] * xOld + A[0][1] * yOld + A[0][2] * thetaOld;
    Ax[1] = A[1][0] * xOld + A[1][1] * yOld + A[1][2] * thetaOld;
    Ax[2] = A[2][0] * xOld + A[2][1] * yOld + A[2][2] * thetaOld;
    

    // STEP 4: Matrix multiplication B * u
    // u = [forwardSpeed, rotationSpeed]
    float Bu[3];
    Bu[0] = B[0][0] * forwardSpeed + B[0][1] * rotationSpeed;
    Bu[1] = B[1][0] * forwardSpeed + B[1][1] * rotationSpeed;
    Bu[2] = B[2][0] * forwardSpeed + B[2][1] * rotationSpeed;
    

    // STEP 5: Add them together (the state space equation)
    x = Ax[0] + Bu[0];
    y = Ax[1] + Bu[1];
    theta = Ax[2] + Bu[2];
    

    // STEP 6: Keep theta between -180° and +180°
    while (theta > PI) {
        theta -= 2.0 * PI;
    }
    while (theta < -PI) {
        theta += 2.0 * PI;
    }

}




//------------------------------------------------------------
// GET X POSITION
// Purpose: Read current X coordinate
//------------------------------------------------------------

float StateSpace::getX() {
    return x;
}




//------------------------------------------------------------
// GET Y POSITION
// Purpose: Read current Y coordinate
//------------------------------------------------------------

float StateSpace::getY() {
    return y;
}




//------------------------------------------------------------
// GET THETA (RADIANS)
// Purpose: Read current heading angle in radians
//------------------------------------------------------------

float StateSpace::getTheta() {
    return theta;
}




//------------------------------------------------------------
// GET THETA (DEGREES)
// Purpose: Read current heading angle in degrees (easier to understand)
//------------------------------------------------------------

float StateSpace::getThetaDegrees() {
    return theta * 180.0 / PI;
}




//------------------------------------------------------------
// PRINT STATE
// Purpose: Show current position on Serial Monitor
//------------------------------------------------------------

void StateSpace::printState() {
    Serial.print("Position: [");
    Serial.print("x=");
    Serial.print(x, 4);
    Serial.print("m, y=");
    Serial.print(y, 4);
    Serial.print("m, angle=");
    Serial.print(getThetaDegrees(), 2);
    Serial.println("°]");
}




//------------------------------------------------------------
// PRINT MATRICES
// Purpose: Show the A and B matrices 
//------------------------------------------------------------

void StateSpace::printMatrices() {
    Serial.println("========================================");
    Serial.println("A Matrix (State Transition)");
    Serial.println("========================================");
    
    for (int i = 0; i < 3; i++) {
        Serial.print("[ ");
        for (int j = 0; j < 3; j++) {
            Serial.print(A[i][j], 2);
            if (j < 2) Serial.print("\t");
        }
        Serial.println(" ]");
    }
    
    Serial.println("\n========================================");
    Serial.println("B Matrix (Control Input)");
    Serial.println("Linearized at current theta");
    Serial.println("========================================");
    
    for (int i = 0; i < 3; i++) {
        Serial.print("[ ");
        for (int j = 0; j < 2; j++) {
            Serial.print(B[i][j], 4);
            if (j < 1) Serial.print("\t");
        }
        Serial.println(" ]");
    }
    
    Serial.println("========================================\n");
}

