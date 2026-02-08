//please note i tested this on main but added it here to keep it for future uses or for reference so if it needs any fixing considering the files do it.

#include <Arduino.h>
#include "StateSpace.h"


//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: Create the state space model
// Tested state space successfully
//------------------------------------------------------------

StateSpace robot;


//------------------------------------------------------------
// SETUP
// Purpose: Run once when ESP32 starts
//------------------------------------------------------------

void setup() {

    Serial.begin(9600);
    delay(2000);  // Wait for Serial Monitor to open
    

    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("STATE SPACE MODEL TEST");
    Serial.println("========================================");
    

    // Initialize the state space model
    robot.initialize();
    robot.setStartPosition(0.0, 0.0, 0.0);  // Start at origin facing North
    

    Serial.println("\nStarting position:");
    robot.printState();
    robot.printMatrices();
    

    //------------------------------------------------------------
    // TEST 1: MOVE FORWARD
    // Purpose: Verify forward movement works
    //------------------------------------------------------------

    Serial.println("\n========================================");
    Serial.println("TEST 1: Move forward 0.2m/s for 1 second");
    Serial.println("Expected: y should increase by 0.2m");
    Serial.println("========================================");
    
    robot.predict(0.2, 0.0, 1.0);  // v=0.2m/s, omega=0rad/s, dt=1.0s
    robot.printState();
    

    //------------------------------------------------------------
    // TEST 2: ROTATE
    // Purpose: Verify rotation works
    //------------------------------------------------------------

    Serial.println("\n========================================");
    Serial.println("TEST 2: Rotate 90° (1.57 rad/s for 1 second)");
    Serial.println("Expected: angle should change to ~90°");
    Serial.println("========================================");
    
    robot.predict(0.0, 1.57, 1.0);  // v=0m/s, omega=1.57rad/s, dt=1.0s
    robot.printState();
    robot.printMatrices();  // Show updated B matrix
    

    //------------------------------------------------------------
    // TEST 3: MOVE FORWARD AGAIN
    // Purpose: Verify movement at 90° works correctly
    //------------------------------------------------------------

    Serial.println("\n========================================");
    Serial.println("TEST 3: Move forward again at 90°");
    Serial.println("Expected: x should increase (moving East now)");
    Serial.println("========================================");
    
    robot.predict(0.2, 0.0, 1.0);
    robot.printState();
    

    //------------------------------------------------------------
    // TEST 4: CONTINUOUS MOVEMENT
    // Purpose: Simulate real robot updating every 0.02 seconds
    //------------------------------------------------------------

    Serial.println("\n========================================");
    Serial.println("TEST 4: Simulate continuous movement");
    Serial.println("Moving forward at 0.1m/s for 2 seconds");
    Serial.println("Updates every 0.02s (50Hz)");
    Serial.println("========================================");
    
    robot.resetToOrigin();
    
    float timeStep = 0.02;  // 20ms updates (50Hz)
    float totalTime = 2.0;   // Run for 2 seconds
    int steps = totalTime / timeStep;
    
    for (int i = 0; i < steps; i++) {
        robot.predict(0.1, 0.0, timeStep);
        
        // Print every 10th update
        if (i % 10 == 0) {
            Serial.print("Time: ");
            Serial.print(i * timeStep, 2);
            Serial.print("s -> ");
            robot.printState();
        }
    }
    

    Serial.println("\n========================================");
    Serial.println("FINAL POSITION:");
    Serial.println("========================================");
    robot.printState();
    
    Serial.println("\n========================================");
    Serial.println("ALL TESTS COMPLETE");
    Serial.println("========================================");

}


//------------------------------------------------------------
// LOOP
// Purpose: Run repeatedly (not used in this test)
//------------------------------------------------------------

void loop() {
    // Nothing here for now
    // State space just tracks position, doesn't control motors yet
}

