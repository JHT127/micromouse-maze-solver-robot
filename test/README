
# Micromouse Test Suite

This directory contains **12 comprehensive tests** organized into unit tests (component validation) and integration tests (system verification).

## 📋 Test Organization

### Unit Tests (Component-Level)
Test individual components in isolation to verify correct operation.

| Test File | Component | Purpose | Use Case |
|-----------|-----------|---------|----------|
| **FloodFill_test.cpp** | FloodFill Algorithm | Validate maze solving algorithm (BFS, distance field, pathfinding) | New to the maze algorithm? Start here. |
| **Encoders_Motors_test.cpp** | Motors + Encoders | Basic motor control and encoder feedback | Hardware setup verification |
| **PIDController_test.cpp** | PID Control System | Velocity regulation with tuned coefficients | PID tuning and velocity control |
| **MPU6050_test.cpp** | IMU (Gyroscope) | Sensor initialization, calibration, angle tracking | Gyro integration debugging |
| **StateSpace_test.cpp** | Position Tracking | Robot position model (x, y, θ) | Understanding state representation |
| **LIDARSensor_test.cpp** | Distance Sensors | LIDAR initialization and ranging | Obstacle detection setup |

**Duration**: Each ~5-10 seconds
**Difficulty**: Easy - Good starting point for new developers

### Integration Tests (System-Level)
Test how components work together to achieve higher-level functionality.

| Test File | Components | Purpose | Use Case |
|-----------|-----------|---------|----------|
| **MotionController_test.cpp** | Motors + Encoders + PIDs + IMU + StateSpace | Test forward movement and rotation commands | Motion system validation |
| **MazeNavigator_test.cpp** | All components + FloodFill | Complete maze solving system in action | Full system integration test |
| **MoveForward_WallDetection_test.cpp** | Motors + LIDARs + MotionController | Forward motion with active wall detection | Safety/navigation verification |
| **moveAndTurn_Test.cpp** | Motors + Sensors + Controllers | Sequential move and turn patterns | Navigation behavior validation |
| **CollisionAvoidance_test.cpp** | Motors + LIDARs + Safety Logic | Emergency stop and avoidance behaviors | Safety system verification |
| **turn_90_test.cpp** | Motors + IMU + MotionController | Precision 90-degree rotations | Rotation accuracy testing |

**Duration**: 10-30 seconds each
**Difficulty**: Moderate - Requires hardware to be properly connected

---

## 🚀 How to Run Tests

### Quick Start (Run One Test)

1. **Choose your test** from the lists above
   
2. **Edit `platformio.ini`** in the project root:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   upload_port = COM4
   monitor_speed = 9600
   
   # Add this line to run a specific test:
   build_src_filter = +<test/FloodFill_test.cpp>
   ```

3. **Upload to ESP32**:
   ```bash
   # Via VS Code: Click "Upload" in PlatformIO Home
   # Or via terminal:
   platformio run -t upload
   ```

4. **View Results** (Monitor Serial Output):
   ```bash
   platformio device monitor --baud 9600
   ```
   Or open VS Code Terminal → PlatformIO: Serial Port Monitor

---

## 📊 Understanding Test Output

### FloodFill_test.cpp Output Example
```
========================================
FLOOD FILL ALGORITHM TEST
========================================

TEST 1: Maze Initialization
========================================
✓ Maze initialized (8x8 grid)
  Boundary walls: added
  Goal cells: (3,3), (3,4), (4,3), (4,4)
  Distance field: calculated
```

### Encoders_Motors_test.cpp Output Example
```
========================================
ENCODER & MOTOR TEST
========================================

Left Motor: 200 PWM → Ticks = 1024, Velocity = 0.38 m/s
Right Motor: 200 PWM → Ticks = 1012, Velocity = 0.36 m/s
✓ Motors operational, encoders tracking

PID Feedback:
  Target: 0.20 m/s | Actual: 0.38 m/s | Error: -0.18
  Correcting...
```

**✓ = Pass**  |  **✗ = Fail**

---

## 🧩 Test Strategy (Recommended Order)

### Phase 1: Foundation Validation (30 min)
1. **Encoders_Motors_test.cpp** ← Start here
2. **MPU6050_test.cpp**
3. **LIDARSensor_test.cpp**

*Goal*: Verify all hardware sensors are connected and responding

### Phase 2: Control Systems (15 min)
4. **PIDController_test.cpp**
5. **StateSpace_test.cpp**

*Goal*: Verify control algorithms work correctly

### Phase 3: High-Level Motion (15 min)
6. **MotionController_test.cpp**
7. **turn_90_test.cpp**

*Goal*: Test motion command generation

### Phase 4: Path Planning (10 min)
8. **FloodFill_test.cpp**

*Goal*: Verify maze algorithm

### Phase 5: Full Integration (30 min)
9. **MoveForward_WallDetection_test.cpp**
10. **moveAndTurn_Test.cpp**
11. **CollisionAvoidance_test.cpp**
12. **MazeNavigator_test.cpp** ← Full system test

*Goal*: Complete system verification

---

## 🔧 Troubleshooting Tests

### Test Won't Compile
- **Error**: `"pins.h" not found`
  - Solution: Ensure `include/` directory exists with pins.h

### Serial Output Not Appearing
- Check COM port in `platformio.ini` (should match Device Manager)
- Reset ESP32: Press RST button on board
- Check baud rate: Should be 9600

### Motor Test Fails (No Response)
- **Check**: L298N power supply (7-12V)
- **Check**: Motor GPIO pins match `include/pins.h`
- **Check**: Motor power wires are connected

### LIDAR Test Fails (I2C Not Found)
- **Check**: SDA (GPIO21) and SCL (GPIO22) connected with pull-up resistors (4.7kΩ)
- **Check**: LIDAR power supply (3.3V)
- **Check**: I2C address selection pins on each LIDAR

### IMU Calibration Takes Too Long
- **Normal**: Calibration (`imu.calibrate(500)`) takes ~5 seconds with sensor motionless
- **Keep still**: Any movement invalidates calibration

---

## 📝 Test Code Structure

Every test follows the same format for readability:

```cpp
#include <Arduino.h>
#include "pins.h"
// ... includes ...

//------------------------------------------------------------
// TEST OBJECTS
// Purpose: Declare hardware objects to test
//------------------------------------------------------------
Motor motor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);

//------------------------------------------------------------
// SETUP
// Purpose: Initialize hardware and print header
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("========================================");
    Serial.println("TEST NAME HERE");
    Serial.println("========================================\n");
    
    // Initialization code...
}

//------------------------------------------------------------
// LOOP
// Purpose: Run test sequence
//------------------------------------------------------------
void loop() {
    // Test logic...
    delay(1000); // Results display time
    
    while(1) delay(1000); // Halt after test
}
```

---

## ✅ Test Success Criteria

### Unit Test Success
- ✓ All output shows ✓ symbols (no ✗)
- ✓ No error messages in serial output
- ✓ Test completes and halts (doesn't restart)

### Integration Test Success
- ✓ Robot executes commands (moves/turns)
- ✓ Sensors respond with expected values
- ✓ No error messages or exceptions
- ✓ Test halts cleanly after completion

---

## 🎓 Using Tests for Learning

Each test doubles as a **code example**:

1. **Want to understand motors?** → Read `Encoders_Motors_test.cpp`
2. **Want to understand PID?** → Read `PIDController_test.cpp`
3. **Want to understand maze solving?** → Read `FloodFill_test.cpp`
4. **Want to understand motion control?** → Read `MotionController_test.cpp`

The tests are intentionally simple and well-commented to serve as tutorials.

---

## 📚 Additional Resources

- Component details: See [../READMEs/](../READMEs/) directory
- Hardware setup: See [../include/pins.h](../include/pins.h)
- Main application: See [../src/main.cpp](../src/main.cpp)
- PlatformIO docs: https://docs.platformio.org/

---

## 🔄 Running All Tests Sequentially

To validate the entire system:

```bash
# Instead of editing platformio.ini 12 times, run each test manually:

for test in FloodFill_test Encoders_Motors_test PIDController_test \
            MPU6050_test StateSpace_test LIDARSensor_test \
            MotionController_test turn_90_test MoveForward_WallDetection_test \
            moveAndTurn_Test CollisionAvoidance_test MazeNavigator_test; do
    
    echo "Running $test..."
    # Edit platformio.ini to include +<test/${test}.cpp>
    # Run upload and monitor
    # Check results
done
```

Or use the provided test runner scripts (if available).

---

**Last Updated**: April 24, 2026
**Test Count**: 12 comprehensive tests
**Coverage**: Component + Integration testing
**Status**: All tests passing ✓
