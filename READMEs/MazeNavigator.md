# Maze Navigator - Full System Integration (Day 7b)

## What is this?

The top-level coordinator that ties everything together. It orchestrates:
1. **MotionController** - for movement execution
2. **LIDARSensor** (3x) - for wall detection
3. **FloodFill** - for pathfinding
4. **All lower-level components** - motors, encoders, PID, IMU

This is the main "brain" that runs the complete maze exploration.

## How It Works

### Main Loop

```
1. Sense: Read LIDAR sensors
2. Update: Add walls to maze map
3. Plan: Calculate next direction using FloodFill
4. Act: Move robot in that direction
5. Repeat until goal reached
```

## How to Use

```cpp
#include "MazeNavigator.h"

// Create all components (see main.cpp for full setup)
MotionController motion(...);
LIDARSensor front(...);
LIDARSensor left(...);
LIDARSensor right(...);
FloodFill maze;

MazeNavigator navigator(&motion, &front, &left, &right, &maze);

void setup() {
    Serial.begin(115200);
    
    // Initialize all hardware
    motion.initialize();
    maze.initialize();
    
    // Set starting position
    navigator.initialize(Cell(0, 0), NORTH);
    
    // Start autonomous exploration
    navigator.exploreMode();
}

void loop() {
    // Just idle - exploration handles itself
    delay(1000);
}
```

## Key Functions

**Setup:**
- `initialize(startCell, heading)` - Set starting position and direction

**Main Control:**
- `exploreMode()` - Run full maze exploration until goal reached
- `speedRun()` - (Not yet implemented) Fast run with known map
- `step()` - Single exploration step (for testing)

**Status:**
- `isAtGoal()` - Check if reached center
- `getCurrentCell()` - Get current position
- `getCurrentHeading()` - Get current heading

## Exploration Mode Algorithm

```cpp
while (!at goal) {
    // 1. SENSE - Read all LIDAR sensors
    detectWalls()
    
    // 2. UPDATE - Add walls to maze
    floodFill->updateWalls()
    
    // 3. PLAN - Get next move
    nextDir = floodFill->getNextMove()
    
    // 4. ACT - Execute movement
    executeMove(nextDir)
    
    // 5. VERIFY - Check position
    updatePosition()
}
```

## Coordinate System

```
Current heading example:

NORTH (0):    →Y
   |
   +--→X EAST (1)

Y (rows)
^
|  (3,7) - top
|
+--→ X (columns)
(0,0) - bottom-left
```

**Directions:**
- NORTH = 0: Moving forward (+Y)
- EAST = 1: Moving right (+X)
- SOUTH = 2: Moving backward (-Y)
- WEST = 3: Moving left (-X)

## Position Tracking

After each movement, robot updates its cell:

```cpp
if (heading == NORTH)   currentCell.y++;
if (heading == EAST)    currentCell.x++;
if (heading == SOUTH)   currentCell.y--;
if (heading == WEST)    currentCell.x--;
```

## Wall Detection Integration

Three sensors provide local wall information:

```
        FRONT
         |
    LEFT-+-RIGHT
         |
      BACK

Front sensor → wall ahead (in heading direction)
Left sensor  → wall to left (perpendicular)
Right sensor → wall to right (perpendicular)
```

FloodFill converts relative (front/left/right) to absolute (N/E/S/W).

## Movement Execution

Each move involves:
1. Calculate turn needed to face target direction
2. Execute turn using MotionController (gyro-based)
3. Move forward one cell (20cm) using MotionController (encoder-based)
4. Update position tracking

## Serial Output Example

```
=== EXPLORATION MODE ===

--- Step 1 at cell (0, 0) ---
Walls - Front: 0 Right: 1 Left: 0
Current heading: 0 Next direction: 1
Turning right...
Moving forward (0.20 m)...
New position: (1, 0)

--- Step 2 at cell (1, 0) ---
Walls - Front: 0 Right: 0 Left: 1
Current heading: 1 Next direction: 2
Turning right...
Moving forward (0.20 m)...
New position: (1, 1)

...

=== GOAL REACHED! ===
```

## Integration Points

**Receives from:**
- MotionController → movement completion status
- LIDARSensor → wall detection (3 sensors)
- FloodFill → next direction to move

**Sends to:**
- MotionController → move forward, turn left/right
- FloodFill → wall data for map updates

## Non-Blocking Design

MazeNavigator uses non-blocking motion:

```cpp
void exploreMode() {
    bool motionInProgress = false;
    
    while (!goal) {
        if (!motionInProgress) {
            // Start new move
            executeMove(nextDir);
            motionInProgress = true;
        }
        
        // Poll motion status
        if (!motionController->isMoving()) {
            // Movement complete
            updatePosition();
            motionInProgress = false;
        }
        
        delay(50);  // Don't hog CPU
    }
}
```

This allows other tasks to run during movement.

## Safety Features

1. **Step Limit**: Maximum 200 steps to prevent infinite loops
2. **Emergency Stop**: Calls `motionController->stop()` if error
3. **Wall Checking**: Prevents moving through walls
4. **Goal Detection**: Stops when 4 center cells reached

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Robot doesn't move | Check motion initialization, LIDAR, IMU |
| Explores infinitely | Verify goal cells, check wall detection |
| Hits walls | Calibrate LIDAR threshold |
| Moves wrong direction | Check coordinate system, heading tracking |
| Goes in circles | Verify wall updating, FloodFill algorithm |

## Testing Sequence

1. **Test individually first:**
   - MotionController alone
   - LIDARSensor readings
   - FloodFill algorithm

2. **Test integration:**
   - Run exploreMode() in a known simple maze
   - Monitor Serial output
   - Verify position tracking
   - Check wall detection

3. **Full maze:**
   - Place robot at (0,0) facing NORTH
   - Run exploreMode()
   - Should autonomously reach center

## Expected Behavior

**Good Exploration:**
- Moves in logical patterns
- Detects walls and adjusts
- Reaches goal in 30-50 steps
- No crashes or infinite loops

**Symptoms of Issues:**
- Rapid direction changes → Bad wall detection
- Repeated same path → Maze not updating
- Stuck in corner → Goal detection failure

## Next Steps

After basic exploration works:
1. **Optimize**: Reduce unnecessary moves
2. **Speed Run**: Use known map for faster path
3. **Diagonal Movement**: Cut corners (advanced)
4. **Sensor Fusion**: Combine encoder + IMU data

## Performance Metrics

```
Expected metrics:
- Exploration time: ~30-60 seconds
- Steps to goal: 30-50 cell moves
- Positioning accuracy: ±5mm
- Turn accuracy: ±2 degrees
```

## Code Structure

```
MazeNavigator
├─ exploreMode()    - Main autonomous loop
├─ step()           - Single step (testing)
├─ detectWalls()    - Read LIDAR sensors
├─ executeMove()    - Turn + forward motion
├─ updatePosition() - Track location
└─ Status queries
```

## Architecture Summary

```
┌─ MazeNavigator
│  ├─ MotionController
│  │  ├─ Motor (L298N)
│  │  ├─ Encoder (feedback)
│  │  ├─ PIDController
│  │  ├─ MPU6050Driver
│  │  └─ StateSpace
│  ├─ LIDARSensor (3x)
│  │  ├─ Front @ 0x30
│  │  ├─ Left @ 0x31
│  │  └─ Right @ 0x32
│  └─ FloodFill
│     ├─ Maze map
│     └─ Distance field
```

---

**Why this file exists:**
- System integration overview
- Algorithm explanation
- Testing and troubleshooting guide
- Performance expectations
