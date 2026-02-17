# FloodFill Maze Algorithm (Day 7a)

## What is this?

The brain of the maze solver. Implements the Flood Fill algorithm to:
1. Learn the maze layout by detecting walls
2. Calculate the shortest path to the goal
3. Decide which direction the robot should move next

Imagine filling the maze from the goal with water - it spreads to all reachable cells. Cells closer to the goal have lower "water level". Robot always moves to the cell with the lowest level.

## How It Works

### Representation
- **8×8 grid** of cells (64 total)
- Each cell can have walls on 4 sides (N, E, S, W)
- **Goal**: Center 4 cells at (3,3), (3,4), (4,3), (4,4)
- **Wall encoding**: 4-bit number (N=1, E=2, S=4, W=8)

### Algorithm Stages

**Stage 1: Initialize**
```
- Set all boundary walls (maze edges)
- Set goal cells to distance 0
- All other cells to infinity
```

**Stage 2: Detect Walls**
```
- Read LIDAR sensors
- Convert front/left/right to absolute directions
- Add walls to maze map
```

**Stage 3: Calculate Distances**
```
- BFS flood fill from goals
- Each iteration, cells update from neighbors
- Repeat until all cells have minimum distance
```

**Stage 4: Get Next Move**
```
- Look at all 4 neighbors
- Skip if wall blocks the way
- Choose neighbor with lowest distance
- Return that direction
```

## How to Use

```cpp
#include "FloodFill.h"

FloodFill maze;

void setup() {
    Serial.begin(115200);
    maze.initialize();
}

void loop() {
    // Current robot position
    Cell currentPos(x, y);
    Direction heading = NORTH;
    
    // Update walls based on sensors
    bool frontWall = true;   // From LIDAR
    bool rightWall = false;
    bool leftWall = false;
    
    maze.updateWalls(currentPos, frontWall, rightWall, leftWall, heading);
    
    // Get next move direction
    Direction nextMove = maze.getNextMove(currentPos, heading);
    
    // Check if at goal
    if (maze.isAtGoal(currentPos)) {
        Serial.println("Goal reached!");
    }
}
```

## Key Structures

### Cell
```cpp
struct Cell {
    int x;  // 0-7 (horizontal)
    int y;  // 0-7 (vertical)
};
```

### Direction Enum
```cpp
enum Direction {
    NORTH = 0,  // +Y direction (up)
    EAST = 1,   // +X direction (right)
    SOUTH = 2,  // -Y direction (down)
    WEST = 3    // -X direction (left)
};
```

### Wall Bitfield
```cpp
walls[x][y] format:
- Bit 0 (=1):   North wall
- Bit 1 (=2):   East wall
- Bit 2 (=4):   South wall
- Bit 3 (=8):   West wall

Example: walls[3][3] = 0x05 means:
- 0x05 = binary 0101
- North wall (bit 0) = 1 ✓
- East wall (bit 1) = 0 ✗
- South wall (bit 2) = 1 ✓
- West wall (bit 3) = 0 ✗
```

## Functions

**Setup:**
- `initialize()` - Set boundary walls, initialize maze
- `reset()` - Clear visited flag but keep walls

**Wall Management:**
- `setWall(cell, direction, exists)` - Add/remove wall
- `updateWalls(cell, front, right, left, heading)` - Update from sensors

**Pathfinding:**
- `getNextMove(current, heading)` - Get best direction
- `isAtGoal(cell)` - Check if reached center
- `calculateDistances()` - Recalculate distance field

**Utility:**
- `relativeToAbsolute(relative, heading)` - Convert directions
- `getTurnAngle(from, to)` - Calculate turn needed

## Maze Layout

```
     0   1   2   3   4   5   6   7
   +---+---+---+---+---+---+---+---+
 7 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 6 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 5 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 4 |   |   |   | G | G |   |   |   |
   +---+---+---+---+---+---+---+---+
 3 |   |   |   | G | G |   |   |   |
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 0 | S |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
```

- S = Start (0,0)
- G = Goal (3,3), (3,4), (4,3), (4,4)

## Example: Distance Field

After reaching the goal, the maze looks like:

```
     0   1   2   3   4   5   6   7
   +---+---+---+---+---+---+---+---+
 7 | 8 | 7 | 6 | 7 | 8 | 9 |10 |11 |
   +---+---+---+---+---+---+---+---+
 6 | 7 | 6 | 5 | 6 | 7 | 8 | 9 |10 |
   +---+---+---+---+---+---+---+---+
 5 | 6 | 5 | 4 | 5 | 6 | 7 | 8 | 9 |
   +---+---+---+---+---+---+---+---+
 4 | 5 | 4 | 3 | 1 | 0 | 1 | 2 | 3 |
   +---+---+---+---+---+---+---+---+
 3 | 4 | 3 | 2 | 0 | 1 | 2 | 3 | 4 |
   +---+---+---+---+---+---+---+---+
 2 | 3 | 2 | 1 | 1 | 2 | 3 | 4 | 5 |
   +---+---+---+---+---+---+---+---+
 1 | 2 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
   +---+---+---+---+---+---+---+---+
 0 | 1 | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   +---+---+---+---+---+---+---+---+
```

Numbers show distance to goal (lower = closer).

## Wall Detection Directions

When robot reports walls as front/right/left, they're relative to current heading.

**Conversion:**
```cpp
Heading = NORTH (0)
- Front → NORTH
- Right → EAST
- Left → WEST

Heading = EAST (1)
- Front → EAST
- Right → SOUTH
- Left → NORTH
```

## Testing

Test with `FloodFill_test.cpp`:
- Initialize maze
- Add walls manually
- Verify distance calculation
- Test getNextMove() function
- Check at-goal detection

## Algorithm Complexity

- **Time**: O(N) per update (N = grid size = 64)
- **Space**: O(N) for distance array
- **Iterations**: ~20-30 for full maze exploration

## Common Issues

| Problem | Solution |
|---------|----------|
| Robot goes in circles | Check wall detection and updateWalls() logic |
| Always moves same direction | Verify getNextMove() is being called |
| Robot stops at walls | Check isAtGoal() implementation |
| Walls not added | Verify sensor readings are correct |

## Integration with Other Components

```
MazeNavigator calls:
├─ updateWalls() with LIDAR readings
├─ getNextMove() for direction
└─ isAtGoal() for completion check
```

## Next Steps

After FloodFill works:
1. Combine with MotionController for movement
2. Create MazeNavigator for full exploration
3. Add speed run mode with known map

## References

- Flood Fill algorithm commonly used in maze solving contests
- Typical competition robot uses this or recursive backtracking
- Goal: Reach center in minimal moves (exploration)
- Speed run: Follow optimal path as fast as possible

---

**Why this file exists:**
- Algorithm explanation for reference
- Cell structure and wall encoding
- Integration guide for MazeNavigator
