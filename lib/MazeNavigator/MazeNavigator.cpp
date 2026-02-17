#include "MazeNavigator.h"

//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Store hardware references
//------------------------------------------------------------
MazeNavigator::MazeNavigator(MotionController* mc,
                             LIDARSensor* front,
                             LIDARSensor* left,
                             LIDARSensor* right,
                             FloodFill* ff)
    : motionController(mc),
      frontLidar(front),
      leftLidar(left),
      rightLidar(right),
      floodFill(ff)
{
    currentCell = Cell(0, 0);
    currentHeading = NORTH;
    currentMode = EXPLORATION;
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Set starting state
//------------------------------------------------------------
void MazeNavigator::initialize(Cell start, Direction heading) {
    currentCell = start;
    currentHeading = heading;
    currentMode = EXPLORATION;
    
    floodFill->initialize();
    
    Serial.print("MazeNavigator initialized at cell (");
    Serial.print(currentCell.x);
    Serial.print(", ");
    Serial.print(currentCell.y);
    Serial.print(") heading ");
    Serial.println(currentHeading);
}

//------------------------------------------------------------
// EXPLORE MODE
// Purpose: Autonomously explore maze until goal
//------------------------------------------------------------
void MazeNavigator::exploreMode() {
    Serial.println("=== EXPLORATION MODE ===");
    
    int stepCount = 0;
    bool motionInProgress = false;
    Direction nextDir = NORTH;
    
    while (!isAtGoal()) {
        if (!motionInProgress) {
            stepCount++;
            
            Serial.print("\n--- Step ");
            Serial.print(stepCount);
            Serial.print(" at cell (");
            Serial.print(currentCell.x);
            Serial.print(", ");
            Serial.print(currentCell.y);
            Serial.println(") ---");
            
            // Detect walls with sensors
            detectWalls();
            
            // Get next move from flood fill
            nextDir = floodFill->getNextMove(currentCell, currentHeading);
            
            Serial.print("Current heading: ");
            Serial.print(currentHeading);
            Serial.print(" Next direction: ");
            Serial.println(nextDir);
            
            // Start move
            executeMove(nextDir);
            motionInProgress = true;
        }
        
        // Poll motion status
        if (motionInProgress && !motionController->isMoving()) {
            // Update position after movement completes
            int turnDiff = (nextDir - currentHeading + 4) % 4;
            
            // Update heading based on turn
            if (turnDiff == 1) {
                currentHeading = nextDir;
            } else if (turnDiff == 3) {
                currentHeading = nextDir;
            } else if (turnDiff == 2) {
                currentHeading = nextDir;
            }
            
            updatePosition();
            motionInProgress = false;
        }
        
        // Safety check
        if (stepCount > 200) {
            Serial.println("ERROR: Too many steps, aborting exploration");
            break;
        }
        
        delay(50);
    }
    
    Serial.println("\n=== GOAL REACHED! ===");
    motionController->stop();
}

//------------------------------------------------------------
// SPEED RUN
// Purpose: Follow known optimal path quickly
//------------------------------------------------------------
void MazeNavigator::speedRun() {
    Serial.println("=== SPEED RUN MODE ===");
    
    // For now, just stop
    // Would follow known path with higher speeds
    Serial.println("Speed run not yet implemented");
}

//------------------------------------------------------------
// STEP
// Purpose: Single navigation step (for testing)
//------------------------------------------------------------
void MazeNavigator::step() {
    detectWalls();
    
    Direction nextDir = floodFill->getNextMove(currentCell, currentHeading);
    
    Serial.print("Current: (");
    Serial.print(currentCell.x);
    Serial.print(", ");
    Serial.print(currentCell.y);
    Serial.print(") heading ");
    Serial.print(currentHeading);
    Serial.print(" -> direction ");
    Serial.println(nextDir);
    
    executeMove(nextDir);
}

//------------------------------------------------------------
// DETECT WALLS
// Purpose: Read LIDAR sensors and update maze
//------------------------------------------------------------
void MazeNavigator::detectWalls() {
    bool frontWall = frontLidar->isWallDetected();
    bool rightWall = rightLidar->isWallDetected();
    bool leftWall = leftLidar->isWallDetected();
    
    Serial.print("Walls - Front: ");
    Serial.print(frontWall);
    Serial.print(" Right: ");
    Serial.print(rightWall);
    Serial.print(" Left: ");
    Serial.println(leftWall);
    
    floodFill->updateWalls(currentCell, frontWall, rightWall, leftWall, currentHeading);
}

//------------------------------------------------------------
// EXECUTE MOVE
// Purpose: Move to adjacent cell in specified direction
//------------------------------------------------------------
void MazeNavigator::executeMove(Direction nextDir) {
    // Calculate turn needed
    int turnDiff = (nextDir - currentHeading + 4) % 4;
    
    // Execute turn if needed
    if (turnDiff == 1) {
        // Turn right 90°
        Serial.println("Turning right...");
        motionController->turnRight(PI / 2);
    } else if (turnDiff == 3) {
        // Turn left 90°
        Serial.println("Turning left...");
        motionController->turnLeft(PI / 2);
    } else if (turnDiff == 2) {
        // Turn 180°
        Serial.println("Turning 180...");
        motionController->turnRight(PI);
    }
    
    // Move forward
    Serial.print("Moving forward (");
    Serial.print(CELL_SIZE);
    Serial.println(" m)...");
    motionController->moveForward(CELL_SIZE);
}

//------------------------------------------------------------
// UPDATE POSITION
// Purpose: Calculate new cell based on movement
//------------------------------------------------------------
void MazeNavigator::updatePosition() {
    switch (currentHeading) {
        case NORTH:
            currentCell.y++;
            break;
        case EAST:
            currentCell.x++;
            break;
        case SOUTH:
            currentCell.y--;
            break;
        case WEST:
            currentCell.x--;
            break;
    }
    
    Serial.print("New position: (");
    Serial.print(currentCell.x);
    Serial.print(", ");
    Serial.print(currentCell.y);
    Serial.println(")");
}

//------------------------------------------------------------
// IS AT GOAL
// Purpose: Check if reached center
//------------------------------------------------------------
bool MazeNavigator::isAtGoal() {
    return floodFill->isAtGoal(currentCell);
}

//------------------------------------------------------------
// GET CURRENT CELL
// Purpose: Return current position
//------------------------------------------------------------
Cell MazeNavigator::getCurrentCell() {
    return currentCell;
}

//------------------------------------------------------------
// GET CURRENT HEADING
// Purpose: Return current heading
//------------------------------------------------------------
Direction MazeNavigator::getCurrentHeading() {
    return currentHeading;
}
