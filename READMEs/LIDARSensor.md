# LIDAR Distance Sensor (Day 6)

## What is this?

Reads three VL53L0X Time-of-Flight (ToF) distance sensors to detect walls around the robot. Each sensor measures distance and reports if a wall is present.

**3 Sensors:**
- Front LIDAR → detects walls ahead
- Left LIDAR → detects walls to the left
- Right LIDAR → detects walls to the right

## Hardware Connections

**Each VL53L0X has:**
- VCC → 3.3V
- GND → GND
- SDA → ESP32 pin 21 (shared I2C)
- SCL → ESP32 pin 22 (shared I2C)
- XSHUT → Control pin (GPIO)

**XSHUT Pins:**
- Front LIDAR → pin 16
- Left LIDAR → pin 18
- Right LIDAR → pin 23

**Important Notes:**
1. Add 10kΩ pullup resistors on SDA/SCL
2. All sensors share I2C bus but have different addresses (0x30, 0x31, 0x32)
3. XSHUT pins allow turning sensors on/off individually

## How to Use

```cpp
#include "LIDARSensor.h"

// Create sensor objects
LIDARSensor frontLidar(16, 17);   // XSHUT, INT pins
LIDARSensor leftLidar(18, 19);
LIDARSensor rightLidar(23, 5);

void setup() {
    Serial.begin(115200);
    
    // IMPORTANT: Disable all sensors first
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(50);
    
    // Enable and address one at a time
    frontLidar.enable();
    delay(50);
    frontLidar.initialize(0x30);
    
    leftLidar.enable();
    delay(50);
    leftLidar.initialize(0x31);
    
    rightLidar.enable();
    delay(50);
    rightLidar.initialize(0x32);
}

void loop() {
    // Read distances
    int frontDist = frontLidar.getDistanceMM();
    int leftDist = leftLidar.getDistanceMM();
    int rightDist = rightLidar.getDistanceMM();
    
    Serial.print("Front: ");
    Serial.print(frontDist);
    Serial.print("mm  Left: ");
    Serial.print(leftDist);
    Serial.print("mm  Right: ");
    Serial.print(rightDist);
    Serial.println("mm");
    
    // Check for walls
    if (frontLidar.isWallDetected()) {
        Serial.println("Wall ahead!");
    }
    
    delay(100);
}
```

## Functions

**Setup:**
- `initialize(newAddress)` - Assign unique I2C address (0x30/0x31/0x32)
- `enable()` - Set XSHUT high to power on
- `disable()` - Set XSHUT low to power off

**Reading:**
- `getDistanceMM()` - Distance in millimeters
- `getDistanceMeters()` - Distance in meters (mm / 1000)
- `isWallDetected()` - Returns true if distance < 150mm

**Status:**
- `isReady()` - Check if initialized

## Key Parameters

```
Sensor Type:        VL53L0X (Time-of-Flight)
Default Address:    0x29 (all sensors start here!)
Final Addresses:    0x30 (front), 0x31 (left), 0x32 (right)
Distance Range:     5cm to 2m
Wall Threshold:     15cm (0.15m = 150mm)
I2C Bus:            Shared with MPU6050
Polling Speed:      ~10 measurements per second
```

## Critical: Sequential Initialization

**All VL53L0X sensors start at address 0x29, causing an I2C conflict!**

**Solution**: Initialize one at a time using XSHUT pins:

```cpp
// Step 1: Disable all
frontLidar.disable();  // Pull XSHUT low
leftLidar.disable();
rightLidar.disable();
delay(50);  // Wait for power-down

// Step 2: Enable and address sequentially
frontLidar.enable();      // Pull XSHUT high for front
delay(50);
frontLidar.initialize(0x30);  // Now it's at 0x30
// Front is now safe, other two still off

leftLidar.enable();
delay(50);
leftLidar.initialize(0x31);
// Left is now at 0x31

rightLidar.enable();
delay(50);
rightLidar.initialize(0x32);
// All three at unique addresses - no conflict!
```

## Wall Detection Threshold

Default threshold is 15cm (150mm):

```cpp
// Walls closer than 15cm are detected
bool wall = frontLidar.isWallDetected();

// To change threshold, modify in header:
static constexpr float WALL_THRESHOLD = 0.15;  // 15cm
```

## Distance Readings

```
Distance < 50mm:   Sensor error / out of range
50mm - 150mm:      Wall detected
150mm - 1000mm:    Open space
> 1000mm (1m):     No wall (far away)
```

## Coordinate Directions

```
     FRONT
      |
LEFT--+--RIGHT
      |
    BACK
```

Sensor placement on robot:
- Front points forward (direction of travel)
- Left points to the left
- Right points to the right

## I2C Bus Sharing

Both MPU6050 and LIDARs share I2C bus:

```
I2C Bus (pins 21, 22)
    ├─ MPU6050      @ 0x68
    ├─ Front LIDAR  @ 0x30
    ├─ Left LIDAR   @ 0x31
    └─ Right LIDAR  @ 0x32
```

No conflicts as long as addresses are unique.

## Testing

Test with `LIDARSensor_test.cpp`:
- Initialize each sensor individually
- Verify distance readings in open space
- Place hand in front of each sensor
- Verify wall detection threshold
- Check measurements are stable

## Common Issues

| Problem | Solution |
|---------|----------|
| "Sensor not found at 0x29" | Check I2C wiring, verify pullup resistors |
| Wrong I2C address | Run sequential init (disable all first) |
| Inconsistent readings | Check XSHUT pin connections |
| High distance values | Sensor pointing at reflective surface |
| Low distance values | Sensor pointing at dark/non-reflective surface |

## Measurement Accuracy

```
Close range (5-20cm):    ±5cm error
Mid range (20-100cm):    ±3cm error
Far range (100cm+):      ±10% error
```

For a 20cm maze cell, expect ±2-3cm measurement uncertainty.

## Power Consumption

- Off (XSHUT low):    < 1µA
- Idle (XSHUT high):  < 5mA
- Measuring:          ~15mA each
- All three running:  ~45mA total

## Triggering Measurements

Each call to `getDistanceMM()`:
1. Triggers a measurement
2. Waits for completion (~30ms typical)
3. Reads result
4. Returns distance

This is why LIDAR calls should not be in tight loops.

## Next Steps

After LIDAR works:
1. Use readings to detect maze walls
2. Feed wall data to FloodFill algorithm
3. Autonomous maze exploration

---

**Why this file exists:**
- I2C addressing strategy explanation
- Sequential initialization guide
- Integration reference for maze solver
