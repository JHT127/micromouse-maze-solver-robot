#include "FloodFill.h"

//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Initialize maze representation
//------------------------------------------------------------
FloodFill::FloodFill() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            walls[x][y] = 0;
            distances[x][y] = 0;
            visited[x][y] = false;
        }
    }
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Set up maze with boundary walls
//------------------------------------------------------------
void FloodFill::initialize() {
    // Add outer boundary walls
    for (int x = 0; x < MAZE_SIZE; x++) {
        walls[x][0] |= 0x04;        // South wall at bottom
        walls[x][MAZE_SIZE-1] |= 0x01;  // North wall at top
    }
    for (int y = 0; y < MAZE_SIZE; y++) {
        walls[0][y] |= 0x08;        // West wall at left
        walls[MAZE_SIZE-1][y] |= 0x02;  // East wall at right
    }
    
    calculateDistances();
    Serial.println("FloodFill initialized");
}

//------------------------------------------------------------
// RESET
// Purpose: Clear maze state but keep walls
//------------------------------------------------------------
void FloodFill::reset() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            visited[x][y] = false;
        }
    }
    calculateDistances();
}

//------------------------------------------------------------
// SET WALL
// Purpose: Add or remove wall
//------------------------------------------------------------
void FloodFill::setWall(Cell cell, Direction dir, bool exists) {
    if (!isValid(cell.x, cell.y)) return;
    
    if (exists) {
        walls[cell.x][cell.y] |= (1 << dir);
    } else {
        walls[cell.x][cell.y] &= ~(1 << dir);
    }
}

//------------------------------------------------------------
// UPDATE WALLS
// Purpose: Add walls detected by LIDARs
//------------------------------------------------------------
void FloodFill::updateWalls(Cell cell, bool front, bool right, bool left, Direction heading) {
    if (!isValid(cell.x, cell.y)) return;
    
    // Convert relative walls to absolute directions
    Direction frontDir = heading;
    Direction rightDir = (Direction)((heading + 1) % 4);
    Direction leftDir = (Direction)((heading + 3) % 4);
    
    if (front) {
        setWall(cell, frontDir, true);
    }
    if (right) {
        setWall(cell, rightDir, true);
    }
    if (left) {
        setWall(cell, leftDir, true);
    }
    
    // Recalculate distances after wall update
    calculateDistances();
}

//------------------------------------------------------------
// GET NEXT MOVE
// Purpose: Find lowest distance neighbor
//------------------------------------------------------------
Direction FloodFill::getNextMove(Cell current, Direction heading) {
    if (!isValid(current.x, current.y)) return NORTH;
    
    Direction bestDir = NORTH;
    uint16_t bestDistance = 0xFFFF;
    
    // Check all 4 neighbors
    for (int dir = 0; dir < 4; dir++) {
        Direction d = (Direction)dir;
        
        // Skip if wall blocks this direction
        if (walls[current.x][current.y] & (1 << d)) {
            continue;
        }
        
        Cell neighbor = getNeighbor(current, d);
        if (isValid(neighbor.x, neighbor.y)) {
            if (distances[neighbor.x][neighbor.y] < bestDistance) {
                bestDistance = distances[neighbor.x][neighbor.y];
                bestDir = d;
            }
        }
    }
    
    return bestDir;
}

//------------------------------------------------------------
// IS AT GOAL
// Purpose: Check if at center of maze
//------------------------------------------------------------
bool FloodFill::isAtGoal(Cell current) {
    for (int i = 0; i < 4; i++) {
        if (current == goals[i]) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------
// IS VALID
// Purpose: Check if cell is in bounds
//------------------------------------------------------------
bool FloodFill::isValid(int x, int y) {
    return x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE;
}

//------------------------------------------------------------
// IS GOAL
// Purpose: Check if cell is a goal cell
//------------------------------------------------------------
bool FloodFill::isGoal(Cell cell) {
    for (int i = 0; i < 4; i++) {
        if (cell == goals[i]) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------
// HAS WALL
// Purpose: Check if wall exists in direction
//------------------------------------------------------------
bool FloodFill::hasWall(Cell cell, Direction dir) {
    if (!isValid(cell.x, cell.y)) return true;
    return (walls[cell.x][cell.y] & (1 << dir)) != 0;
}

//------------------------------------------------------------
// GET NEIGHBOR
// Purpose: Get cell in specified direction
//------------------------------------------------------------
Cell FloodFill::getNeighbor(Cell cell, Direction dir) {
    return Cell(cell.x + dx[dir], cell.y + dy[dir]);
}

//------------------------------------------------------------
// CALCULATE DISTANCES
// Purpose: Flood fill algorithm from goal
//------------------------------------------------------------
void FloodFill::calculateDistances() {
    // Initialize all distances
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            distances[x][y] = 0xFFFF;
            visited[x][y] = false;
        }
    }
    
    // Set goal cells to 0
    for (int i = 0; i < 4; i++) {
        distances[goals[i].x][goals[i].y] = 0;
    }
    
    // Simple BFS-style flood fill
    bool changed = true;
    int iterations = 0;
    while (changed && iterations < 100) {
        changed = false;
        iterations++;
        
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                Cell current(x, y);
                uint16_t currentDist = distances[x][y];
                
                if (currentDist == 0xFFFF) continue;
                
                // Check all neighbors
                for (int dir = 0; dir < 4; dir++) {
                    Direction d = (Direction)dir;
                    
                    // Skip if wall blocks
                    if (hasWall(current, d)) continue;
                    
                    Cell neighbor = getNeighbor(current, d);
                    if (isValid(neighbor.x, neighbor.y)) {
                        uint16_t neighborDist = distances[neighbor.x][neighbor.y];
                        
                        if (currentDist + 1 < neighborDist) {
                            distances[neighbor.x][neighbor.y] = currentDist + 1;
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------
// RELATIVE TO ABSOLUTE
// Purpose: Convert relative direction to absolute
//------------------------------------------------------------
Direction FloodFill::relativeToAbsolute(Direction relative, Direction heading) {
    return (Direction)((relative + heading) % 4);
}

//------------------------------------------------------------
// GET TURN ANGLE
// Purpose: Calculate turn angle between two headings
//------------------------------------------------------------
int FloodFill::getTurnAngle(Direction from, Direction to) {
    int angle = (to - from + 4) % 4;
    
    if (angle == 0) return 0;       // No turn
    if (angle == 1) return 90;      // Right 90°
    if (angle == 2) return 180;     // 180°
    if (angle == 3) return -90;     // Left 90° (or -90)
    
    return 0;
}
