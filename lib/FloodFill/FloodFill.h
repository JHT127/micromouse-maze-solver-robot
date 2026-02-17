#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <Arduino.h>

//------------------------------------------------------------
// MAZE CONSTANTS
// Purpose: Define maze dimensions
//------------------------------------------------------------
#define MAZE_SIZE 8
#define CELL_SIZE 0.20  // 20cm cells

//------------------------------------------------------------
// DIRECTION ENUM
// Purpose: Robot orientation
//------------------------------------------------------------
enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

//------------------------------------------------------------
// CELL STRUCTURE
// Purpose: Represent maze cell
//------------------------------------------------------------
struct Cell {
    int x;
    int y;
    
    Cell(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
};

//------------------------------------------------------------
// FLOOD FILL CLASS
// Purpose: Maze solving algorithm
//------------------------------------------------------------

class FloodFill {
private:
    //------------------------------------------------------------
    // MAZE REPRESENTATION
    // Purpose: Store maze state
    //------------------------------------------------------------
    uint8_t walls[MAZE_SIZE][MAZE_SIZE];     // Bitfield: N=1, E=2, S=4, W=8
    uint16_t distances[MAZE_SIZE][MAZE_SIZE];
    bool visited[MAZE_SIZE][MAZE_SIZE];
    
    //------------------------------------------------------------
    // GOAL DEFINITION
    // Purpose: Target cells (center of maze)
    //------------------------------------------------------------
    Cell goals[4] = {
        Cell(3, 3),
        Cell(3, 4),
        Cell(4, 3),
        Cell(4, 4)
    };
    
    //------------------------------------------------------------
    // NEIGHBOR OFFSETS
    // Purpose: 4-direction movement
    //------------------------------------------------------------
    const int dx[4] = {0, 1, 0, -1};  // NORTH, EAST, SOUTH, WEST
    const int dy[4] = {1, 0, -1, 0};
    
    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: Algorithm implementation
    //------------------------------------------------------------
    bool isValid(int x, int y);
    bool isGoal(Cell cell);
    bool hasWall(Cell cell, Direction dir);
    void calculateDistances();
    Cell getNeighbor(Cell cell, Direction dir);

public:
    //------------------------------------------------------------
    // SETUP
    // Purpose: Initialize maze
    //------------------------------------------------------------
    FloodFill();
    void initialize();
    void reset();
    
    //------------------------------------------------------------
    // WALL MANAGEMENT
    // Purpose: Update maze map
    //------------------------------------------------------------
    void setWall(Cell cell, Direction dir, bool exists);
    void updateWalls(Cell cell, bool front, bool right, bool left, Direction heading);
    
    //------------------------------------------------------------
    // PATHFINDING
    // Purpose: Get next move
    //------------------------------------------------------------
    Direction getNextMove(Cell current, Direction heading);
    bool isAtGoal(Cell current);
    
    //------------------------------------------------------------
    // UTILITY
    // Purpose: Convert between types
    //------------------------------------------------------------
    Direction relativeToAbsolute(Direction relative, Direction heading);
    int getTurnAngle(Direction from, Direction to);  // Returns angle in degrees
};

#endif
