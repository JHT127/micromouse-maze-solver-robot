# Architecture & Development Guide

This document explains the software architecture, design patterns, and how to extend the codebase.

## 🏗️ Architecture Overview

The micromouse software is organized into **9 independent layers**, each with clear responsibilities:

```
┌─────────────────────────────────────────┐
│  LAYER 9: MAZE NAVIGATOR                │  Application logic
│  Orchestrates all systems for maze-solving
└────────────────┬────────────────────────┘
                 │
┌─────────────────┴────────────────────────┐
│  LAYER 8: FLOOD FILL ALGORITHM            │  Path planning
│  BFS-based pathfinding on 8×8 grid        │
└─────────────────────────────────────────┘
                 │
┌─────────────────┴────────────────────────────────┐
│  LAYER 7: MOTION CONTROLLER                      │  Motion commands
│  High-level primitives: moveForward(), turnRight()
└────────────────┬────────────────┬────────────────┘
                 │                │
     ┌───────────┴────────────────┴────────────┐
     │                                         │
┌────┴──────────────────────┐   ┌─────────────┴──────────────┐
│ LAYER 6: CONTROL SYSTEMS   │   │ LAYER 6: PERCEPTION        │
│ ├─ PID Controllers        │   │ ├─ LIDAR Sensors (3x)      │
│ ├─ StateSpace Tracker     │   │ ├─ MPU6050 IMU             │
│ └─ Feedback Loops         │   │ └─ Sensor Fusion            │
└────┬──────────────────────┘   └─────────────┬──────────────┘
     │                                        │
     │  ┌────────────────────────────────────┘
     │  │
┌────┴──┴─────────────────────────────────────┐
│  LAYER 5: HARDWARE ABSTRACTION               │
│  ├─ Motor (PWM + direction)                 │
│  ├─ Encoder (GPIO + interrupt)              │
│  ├─ I2C Drivers (IMU, LIDAR)                │
│  └─ GPIO Pin Management                     │
└─────────────┬────────────────────────────────┘
              │
┌─────────────┴────────────────────────────────┐
│  LAYER 4: HARDWARE (Physical devices)        │
│  ├─ ESP32 Microcontroller                   │
│  ├─ L298N H-Bridge (motors)                 │
│  ├─ N20 DC Motors + Encoders                │
│  ├─ VL53L0X LIDAR (3x)                      │
│  └─ MPU6050 IMU                              │
└──────────────────────────────────────────────┘
```

## 🔧 Design Patterns Used

### 1. **Layered Architecture**
- Each layer depends only on layers below it
- Lower layers are generic and reusable
- Upper layers are robot-specific

**Example**: `MotionController` (Layer 7) uses `Motor` (Layer 5) but `Motor` knows nothing about motion.

### 2. **Hardware Abstraction**
- Hardware classes encapsulate device communication
- Application code never directly accesses GPIO/I2C
- Easy to swap implementations

**Example**: To use different motors, only modify `lib/Motor/Motor.cpp`, not application code.

### 3. **Composition Over Inheritance**
- `MazeNavigator` contains instances of other classes
- No deep inheritance hierarchies
- Easier to understand and modify

**Example**: `MazeNavigator` has a `MotionController*` pointer, not inheriting from it.

### 4. **Single Responsibility Principle**
- Each class does one thing well
- `Motor` only drives motors, doesn't track position
- `Encoder` only counts ticks, doesn't control motion

## 📚 Library Details

### Layer 5: Hardware Abstraction

#### `Motor.h` / `Motor.cpp`
```cpp
class Motor {
private:
    int pinA, pinB, pinPWM;  // H-bridge pins
public:
    void forward(int pwm);    // 0-255
    void backward(int pwm);   // 0-255
    void stop();
    void brake();
};
```
**Key Points**:
- Abstracts L298N H-bridge complexity
- PWM range: 0-255 (0% to 100%)
- No state tracking (stateless)

#### `Encoder.h` / `Encoder.cpp`
```cpp
class Encoder {
private:
    long tickCount;           // Interrupt-updated
    static Encoder* leftEncoderPtr;  // ISR access
public:
    long getTicks();
    float getDistance();      // In meters
    float getVelocity(float deltaTime);
};
```
**Key Points**:
- Uses hardware interrupts for accuracy
- No polling → always current
- Interrupt-driven (ISR called per encoder tick)

#### `LIDARSensor.h` / `LIDARSensor.cpp`
```cpp
class LIDARSensor {
private:
    int I2C_address;          // 0x30, 0x31, 0x32
public:
    void enable();            // Power on via GPIO
    void initialize(int address);
    uint16_t getRange();      // In mm
    void startContinuous();   // Continuous polling mode
};
```
**Key Points**:
- Multiple sensors, each with unique I2C address
- XSHUT pin for address control
- Range: 50-1200mm (0.05-1.2m)

#### `MPU6050Driver.h` / `MPU6050Driver.cpp`
```cpp
class MPU6050Driver {
private:
    float heading;
    float gyroBias;           // Calibration offset
public:
    bool initialize();        // Returns false if not found
    void calibrate(int samples);  // Zero gyro bias
    float getHeading();       // In degrees (0-360)
    void update();            // Call frequently
};
```
**Key Points**:
- Integrates gyro readings to heading angle
- Must calibrate (keep still 5 sec)
- Output: absolute heading (0=North, 90=East, etc.)

### Layer 6: Control Systems

#### `PIDController.h` / `PIDController.cpp`
```cpp
class PIDController {
private:
    float Kp, Ki, Kd;         // Tuning coefficients
    float integral, lastError;
public:
    PIDController(float kp, float ki, float kd);
    float calculate(float error, float deltaTime);
    void setOutputLimits(float min, float max);
};
```
**Key Points**:
- Generic PID for any process
- Output clamped to [-255, 255] for motor PWM
- For velocity: `error = targetVelocity - actualVelocity`

#### `StateSpace.h` / `StateSpace.cpp`
```cpp
class StateSpace {
private:
    float x, y, theta;        // Robot position
public:
    void update(float linVel, float angVel, float dt);
    float getX();
    float getY();
    float getTheta();         // In radians
};
```
**Key Points**:
- Integrates velocity into position
- Used to track robot location on maze grid
- Theta in radians (0 = East, π/2 = North, π = West, 3π/2 = South)

### Layer 7: Motion Control

#### `MotionController.h` / `MotionController.cpp`
```cpp
class MotionController {
private:
    Motor* leftMotor, *rightMotor;
    Encoder* leftEncoder, *rightEncoder;
    PIDController* leftPID, *rightPID;
    StateSpace* robot;
    // ... state for current motion ...
public:
    void moveForward(float distance);  // Non-blocking
    void turnRight(float angle);       // Non-blocking
    bool isMoving();                   // Check if done
    void update();                     // Call every 20ms
};
```
**Key Points**:
- Non-blocking: returns immediately
- Call `update()` in main loop to drive motion
- Coordinates PID + encoders + IMU

### Layer 8: Path Planning

#### `FloodFill.h` / `FloodFill.cpp`
```cpp
class FloodFill {
private:
    uint8_t walls[8][8];      // Bitfield: N=1, E=2, S=4, W=8
    uint16_t distances[8][8]; // BFS distance from goal
public:
    void updateWalls(Cell cell, bool front, bool right, bool left, Direction heading);
    Direction getNextMove(Cell current, Direction heading);
};
```
**Key Points**:
- Maze grid: 8×8 cells (Japanese Micromouse standard)
- BFS calculates shortest path from current position to goal
- Wall encoding: bitwise (each direction is one bit)

### Layer 9: System Integration

#### `MazeNavigator.h` / `MazeNavigator.cpp`
```cpp
class MazeNavigator {
private:
    MotionController* motion;
    LIDARSensor* frontLidar, *leftLidar, *rightLidar;
    FloodFill* maze;
public:
    void initialize(Cell start, Direction heading);
    void step();              // Call every 20ms in main loop
    bool isComplete();        // True when maze solved
};
```
**Key Points**:
- Orchestrates all subsystems
- Main loop: Sense → Plan → Act
- Two modes: exploration (learning map) and speed run (known map)

## 🔄 Data Flow Example: Moving Forward

```
main.cpp:setup()
  └─> navigator.initialize(Cell(0,0), NORTH)
  
main.cpp:loop() [every 20ms]
  └─> navigator.step()
       ├─> frontLidar.getRange()           [Sense]
       ├─> leftLidar.getRange()
       ├─> rightLidar.getRange()
       │
       ├─> floodFill.updateWalls(...)      [Plan]
       ├─> direction = floodFill.getNextMove(...)
       │
       ├─> motion.moveForward(distance)    [Act]
       │   ├─> leftMotor.forward(pwm)
       │   ├─> rightMotor.forward(pwm)
       │   └─> motion.update()
       │       ├─> leftEncoder.getVelocity()
       │       ├─> rightEncoder.getVelocity()
       │       ├─> leftPID.calculate(error)
       │       ├─> rightPID.calculate(error)
       │       ├─> imu.getHeading()
       │       └─> robot.update(velocity, heading)
       │
       └─> [Repeat next iteration]
```

## 🔨 How to Add a New Component

### Example: Adding a Compass Sensor

**Step 1: Create Hardware Abstraction** (`lib/CompassSensor/`)
```cpp
// CompassSensor.h
class CompassSensor {
private:
    int I2C_address;  // HMC5883L default: 0x1E
public:
    bool initialize();
    float getHeading();  // Returns 0-360 degrees
};

// CompassSensor.cpp
bool CompassSensor::initialize() {
    Wire.beginTransmission(I2C_address);
    // ... initialization code ...
}

float CompassSensor::getHeading() {
    // Read X, Y from sensor, calculate heading
}
```

**Step 2: Add to Higher Layers**
Update `StateSpace.cpp`:
```cpp
class StateSpace {
private:
    CompassSensor compass;  // Add this
public:
    void update(float linVel, CompassSensor& compass) {  // Add parameter
        // Use compass.getHeading() instead of gyro estimate
    }
};
```

**Step 3: Create Unit Test** (`test/CompassSensor_test.cpp`)
```cpp
#include "CompassSensor.h"

CompassSensor compass;

void setup() {
    Serial.begin(9600);
    compass.initialize();
}

void loop() {
    float heading = compass.getHeading();
    Serial.println(heading);
    delay(100);
}
```

**Step 4: Document** (add to `READMEs/CompassSensor.md`)

**Step 5: Update READMEs**
- Update `README.md` with new layer or component
- Update `test/README.md` with new test
- Update this file with new architecture

## 🎯 Code Style Guidelines

### Commenting Convention
All code uses this header format:
```cpp
//------------------------------------------------------------
// SECTION HEADER
// Purpose: Explanation of why this exists
//------------------------------------------------------------
```

### Variable Naming
- Member variables: camelCase (`motorSpeed`, `encoderCount`)
- Constants: UPPER_CASE (`MOTORS_PER_TICK`, `MAZE_SIZE`)
- Functions: camelCase (`initialize()`, `getDistance()`)

### Class Organization
```cpp
class MyClass {
private:
    // Declare member variables
    // Define helper functions
    
public:
    // Declare public interface
    // Getters first
    // Setters second
    // Action methods third
};
```

### Comments
- **Why** comments above complex code
- **What** comments only if code is unclear
- Avoid obvious comments
```cpp
// Good
if (range < WALL_DISTANCE) {
    // Stop immediately - collision imminent
    motor.stop();
}

// Bad
int x = 5; // Set x to 5
```

## 🧪 Testing Strategy

### Unit Test Pattern
```cpp
// Test only ONE component
// Setup: Create instance, initialize
// Execute: Call methods
// Verify: Check Serial output
// Halt: while(1) delay(1000);
```

### Integration Test Pattern
```cpp
// Test MULTIPLE components working together
// Setup: Initialize all components
// Execute: Run realistic scenario
// Verify: Check combined behavior
// Halt: when complete
```

## 🚀 Performance Considerations

### Critical Timing
- **Main loop**: Must run at 50Hz (20ms)
- **Encoder ISR**: Must complete <100µs
- **Motor PWM**: 20kHz (ESP32 default)

### Memory Usage
- **Stack**: ~4KB used (ESP32 has 2KB SRAM per stack)
- **Heap**: ~50KB available for objects
- **Flash**: ~500KB used for code

## 🔍 Debugging Tips

### Use Serial Monitor
```cpp
Serial.print("Motor PWM: ");
Serial.println(pwmValue);
delay(100);  // Don't flood Serial!
```

### Common Issues
1. **Motors not responding**: Check motor power supply, GPIO pins
2. **Encoder counts wrong**: Verify PPR constant, check GPIO interrupts
3. **LIDAR I2C failing**: Check pull-up resistors, address selection
4. **IMU calibration fails**: Ensure board is level and motionless

## 📚 Further Reading

- `READMEs/` directory - Component-specific details
- `test/` directory - Working code examples
- `src/main.cpp` - Real application code
- Individual `.h` files - API documentation

---

**Last Updated**: April 24, 2026
**Status**: Complete Architecture Documentation
