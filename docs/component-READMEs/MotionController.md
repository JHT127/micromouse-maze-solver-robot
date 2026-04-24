# Motion Controller (Day 5)

## What is this?

High-level motion primitives that combine all low-level components (motors, encoders, PID, gyro, state space) to provide simple commands like `moveForward()` and `turnLeft()`.

Instead of manually controlling PWM values, you just say "move forward 20cm" and the controller handles all the details.

## Hardware Integration

**Uses:**
- Motors (L298N driver)
- Encoders (for velocity feedback)
- PID Controllers (for speed regulation)
- MPU6050 Gyroscope (for angle feedback)
- StateSpace (for odometry tracking)

## How to Use

```cpp
#include "MotionController.h"

// Create components
Motor leftMotor(26, 27, 25);
Motor rightMotor(4, 13, 14);
Encoder leftEncoder(32, 33, true);
Encoder rightEncoder(34, 35, false);
PIDController leftPID(500, 28, 6);
PIDController rightPID(500, 28, 6);
MPU6050Driver imu;
StateSpace robot;

MotionController motion(&leftMotor, &rightMotor,
                        &leftEncoder, &rightEncoder,
                        &leftPID, &rightPID,
                        &imu, &robot);

void setup() {
    Serial.begin(115200);
    
    // Initialize everything
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    imu.initialize();
    imu.calibrate(500);
    
    motion.initialize();
    motion.setForwardSpeed(0.15);  // m/s
    motion.setTurnSpeed(1.57);     // rad/s (90°/s)
}

void loop() {
    // Move 20cm forward
    if (motion.moveForward(0.20)) {
        Serial.println("Forward movement complete");
    }
    
    // Rotate 90° left
    if (motion.turnLeft(PI / 2)) {
        Serial.println("Turn complete");
    }
}
```

## Functions

**Setup:**
- `initialize()` - Reset all components
- `setForwardSpeed(speed)` - Set target speed in m/s (default 0.15)
- `setTurnSpeed(speed)` - Set rotation speed in rad/s (default 1.57 = 90°/s)

**Motion Commands (non-blocking):**
- `moveForward(distance)` - Move straight (returns true when done)
- `moveBackward(distance)` - Reverse motion
- `turnLeft(angle)` - Rotate counterclockwise in radians
- `turnRight(angle)` - Rotate clockwise in radians

**Emergency:**
- `stop()` - Cut motor power immediately

**Status:**
- `isMoving()` - Check if motion in progress

## Key Concept: Non-Blocking

These functions return **immediately** instead of blocking:

```cpp
// WRONG (blocks forever):
motion.moveForward(0.20);
Serial.println("This never runs!");

// RIGHT (non-blocking):
if (motion.moveForward(0.20)) {
    Serial.println("Movement done!");
}
```

You must call the function repeatedly in `loop()` until it returns true.

## Control Parameters

```
Forward Speed:      0.15 m/s
Turn Speed:         1.57 rad/s (90°/s)
Update Rate:        50 Hz (20ms)
Distance Tolerance: 5mm (0.005m)
Angle Tolerance:    2° (0.0349 rad)
```

## PID Tuning

```
Kp: 500    (Proportional gain - response to error)
Ki: 28     (Integral gain - steady-state correction)
Kd: 6      (Derivative gain - damping)
```

These are tuned for typical N20 motors. Adjust if robot:
- **Accelerates too fast** → Lower Kp
- **Doesn't reach target speed** → Raise Ki
- **Oscillates/jerks** → Raise Kd

## How It Works

### Forward Motion
1. Reset encoders to zero
2. Loop:
   - Read wheel velocities
   - PID computes motor commands
   - Update motors
   - Check if distance reached
   - Return true when done

### Turning
1. Reset gyro angle to zero
2. Loop:
   - Read gyro angular velocity
   - PID computes rotation command
   - Set motors in opposite directions
   - Check if angle reached
   - Return true when done

## Integration Example

```cpp
unsigned long lastMove = 0;

void loop() {
    // Execute moves one at a time
    if (millis() - lastMove > 100) {
        // Try forward 20cm
        if (motion.moveForward(0.20)) {
            Serial.println("Moved 20cm");
            lastMove = millis();
        }
        
        // Could also call turn, moveBackward, etc.
    }
}
```

## Testing

Test with `MotionController_test.cpp`:
- Verify forward movement in straight line
- Check 90° turn accuracy
- Test backward motion
- Verify encoder feedback
- Check PID control

## Common Issues

| Problem | Solution |
|---------|----------|
| Robot drifts left/right | PID gains need tuning, check encoder counts |
| Turns are inaccurate | Calibrate IMU with robot still |
| Moves too slow | Increase forwardSpeed or turnSpeed |
| Jerky motion | Decrease Kp, increase Kd |

## Defaults Summary

```cpp
motor speed:    0.15 m/s
rotation speed: 1.57 rad/s (90°/s)
cell size:      0.20 m (20cm) - one maze cell
```

## Next Steps

After motion works:
1. Add LIDAR wall detection
2. Implement maze pathfinding
3. Autonomous exploration

---

**Why this file exists:**
- How to use motion commands
- Parameter tuning reference
- Integration guide for higher-level systems
