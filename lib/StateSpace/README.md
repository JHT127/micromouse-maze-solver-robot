# State Space Model for Micromouse

## What is this?

This is the mathematical foundation for tracking your robot's position.

From the PDF you studied, this implements:
- State vector: [x, y, theta]
- State transition matrix A (identity)
- Control input matrix B (linearized)
- Prediction equation: x_new = A*x + B*u

## How to use

1. Upload to ESP32
2. Open Serial Monitor at 115200 baud
3. Watch the tests run
4. All tests should show expected results

## What the tests do

**Test 1:** Move forward
- Robot should move 0.2m in Y direction

**Test 2:** Rotate 90 degrees
- Angle should change to ~90°

**Test 3:** Move forward at 90°
- Robot should move in X direction (East)

**Test 4:** Continuous simulation
- Shows position updating smoothly over time

## Expected output
```
Position: [x=0.0000m, y=0.0000m, angle=0.00°]
→ After forward: [x=0.0000m, y=0.2000m, angle=0.00°]
→ After rotation: [x=0.0000m, y=0.2000m, angle=90.00°]
→ After forward: [x=0.2000m, y=0.2000m, angle=90.00°]
```

## Coordinate system

- X axis: Left/Right (East/West)
- Y axis: Forward/Back (North/South)
- Theta: 0° = North, 90° = East, 180° = South, 270° = West
- Rotation: Clockwise positive

## Next steps

This is just the foundation. Next you'll:
1. Connect to real encoders (get actual v and omega)
2. Connect to gyroscope (get actual theta)
3. Use this in MotionController to plan movements
4. Eventually add Kalman Filter for sensor fusion
```

**Why this file exists:**
- Documentation for anyone (including future you)
- Explains what the project does
- Shows how to test it

---

## Testing Instructions

### Step 1: Upload the Code

1. Open PlatformIO
2. Connect your ESP32 via USB
3. Click "Upload" button
4. Wait for compilation and upload

### Step 2: Open Serial Monitor

1. Click "Serial Monitor" in PlatformIO
2. Set baud rate to 115200
3. Press reset button on ESP32

### Step 3: Watch the Output

You should see something like this:
```
========================================
STATE SPACE MODEL TEST
========================================

Starting position:
Position: [x=0.0000m, y=0.0000m, angle=0.00°]

A Matrix (State Transition)
[ 1.00    0.00    0.00 ]
[ 0.00    1.00    0.00 ]
[ 0.00    0.00    1.00 ]

B Matrix (Control Input)
[ 0.0000    0.0000 ]
[ 0.0000    0.0000 ]
[ 0.0000    0.0000 ]

========================================
TEST 1: Move forward 0.2m/s for 1 second
Expected: y should increase by 0.2m
========================================
Position: [x=0.0000m, y=0.2000m, angle=0.00°]

✓ PASS: Y increased by 0.2m as expected