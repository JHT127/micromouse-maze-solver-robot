# MPU6050 Gyroscope Driver (Day 4)

## What is this?

Reads the MPU6050 inertial measurement unit (IMU) to track robot heading/angle during movement. This gyroscope measures angular velocity and integrates it to give absolute angle.

## Hardware Connections

**MPU6050 Pins:**
- VCC → 3.3V
- GND → GND
- SDA → ESP32 pin 21 (I2C data)
- SCL → ESP32 pin 22 (I2C clock)

**Important:** Add 10kΩ pullup resistors on SDA and SCL to 3.3V

## How to Use

```cpp
#include "MPU6050Driver.h"

MPU6050Driver imu;

void setup() {
    Serial.begin(115200);
    
    // Initialize sensor
    if (!imu.initialize()) {
        Serial.println("MPU6050 not found!");
        while(1);
    }
    
    // Calibrate (keep robot still!)
    Serial.println("Calibrating... keep robot still");
    imu.calibrate(500);  // Takes ~5 seconds
    Serial.println("Calibration complete");
}

void loop() {
    float dt = 0.02;  // 20ms
    imu.update(dt);
    
    Serial.print("Angle: ");
    Serial.print(imu.getAngleDegrees());
    Serial.println(" degrees");
    
    delay(20);
}
```

## Functions

**Setup:**
- `initialize()` - Configure I2C and verify sensor (returns true if successful)
- `calibrate(samples)` - Measure gyro bias (robot must be still!)

**Reading:**
- `update(dt)` - Integrate gyro reading with time delta
- `getAngle()` - Get angle in radians [-π, π]
- `getAngleDegrees()` - Get angle in degrees [-180, 180]
- `getAngularVelocity()` - Current rotation speed in rad/s

**Reset:**
- `resetAngle()` - Zero out the angle
- `setAngle(angle)` - Manually set angle in radians
- `isReady()` - Check if calibrated

## Key Parameters

```
I2C Address:        0x68
Gyro Range:         ±250°/s
Calibration:        Average of 500 samples
Angle Range:        [-π, π] radians (normalized)
Update Rate:        Variable (pass dt to update)
Sensitivity:        131 LSB/°/s
```

## Coordinate System

- 0° = North (forward)
- 90° = East (right)
- 180° = South (backward)
- -90° (or 270°) = West (left)
- Positive rotation = Clockwise

## Important Notes

1. **Calibration is Critical**: Robot must be completely still during calibration
2. **Drift**: Gyro readings drift over time without sensor fusion
3. **I2C Sharing**: MPU6050 shares I2C bus with LIDAR sensors (different addresses)
4. **Normalization**: Angle automatically wraps to [-π, π] range

## Integration

This component is used by:
- **MotionController**: For accurate turning (90°, 180°)
- **StateSpace**: For rotation tracking

## Testing

Test with `MPU6050_test.cpp` in the test folder:
- Verify initialization
- Check calibration runs
- Monitor angle readings
- Rotate robot 90° and verify reading
- Check angle stability when still

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "MPU6050 not found" | Check I2C wiring, add pullup resistors |
| Angle drifts constantly | Calibrate with robot completely still |
| Wrong angle values | Verify coordinate system (0° = North) |
| No I2C communication | Check pins 21/22 are correctly connected |

## Future Enhancements

- Implement Kalman filter for sensor fusion with encoders
- Add temperature compensation
- Implement FIFO for faster sampling
- Add interrupt-based reading

---

**Why this file exists:**
- Documentation for future reference
- Setup instructions for hardware
- Common troubleshooting guide
