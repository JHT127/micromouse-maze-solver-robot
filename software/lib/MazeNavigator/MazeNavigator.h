#ifndef MAZENAVIGATOR_H
#define MAZENAVIGATOR_H

#include <Arduino.h>
#include "MotionController.h"
#include "LIDARSensor.h"
#include "FloodFill.h"

//------------------------------------------------------------
// MAZE NAVIGATOR CLASS
// Purpose: Top-level maze solving coordination
//------------------------------------------------------------

class MazeNavigator {
private:
    //------------------------------------------------------------
    // HARDWARE REFERENCES
    // Purpose: Access to all components
    //------------------------------------------------------------
    MotionController* motionController;
    LIDARSensor* frontLidar;
    LIDARSensor* leftLidar;
    LIDARSensor* rightLidar;
    FloodFill* floodFill;
    
    //------------------------------------------------------------
    // ROBOT STATE
    // Purpose: Track position in maze
    //------------------------------------------------------------
    Cell currentCell;
    Direction currentHeading;
    
    //------------------------------------------------------------
    // MODE
    // Purpose: Exploration vs speed run
    //------------------------------------------------------------
    enum Mode {
        EXPLORATION,
        SPEED_RUN
    };
    Mode currentMode;
    
    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: Decision making
    //------------------------------------------------------------
    void updatePosition();
    void detectWalls();
    void makeDecision();
    void executeMove(Direction nextDir);

public:
    //------------------------------------------------------------
    // CONSTRUCTOR
    // Purpose: Link all components
    //------------------------------------------------------------
    MazeNavigator(MotionController* mc,
                  LIDARSensor* front,
                  LIDARSensor* left,
                  LIDARSensor* right,
                  FloodFill* ff);
    
    //------------------------------------------------------------
    // SETUP
    // Purpose: Initialize starting state
    //------------------------------------------------------------
    void initialize(Cell start, Direction heading);
    
    //------------------------------------------------------------
    // MAIN CONTROL
    // Purpose: Maze solving loop
    //------------------------------------------------------------
    void exploreMode();     // Run until goal reached
    void speedRun();        // Fast run after exploration
    void step();            // Single step (for testing)
    
    //------------------------------------------------------------
    // STATUS
    // Purpose: Query state
    //------------------------------------------------------------
    bool isAtGoal();
    Cell getCurrentCell();
    Direction getCurrentHeading();
};

#endif
