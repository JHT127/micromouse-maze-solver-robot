# Motor Component

## What is this?

Controls one DC motor using the L298N motor driver.

## Hardware Connections

**L298N Pins:**
- IN1 → ESP32 pin (direction control)
- IN2 → ESP32 pin (direction control)
- ENA → ESP32 pin (speed control via PWM)
- Motor output → DC motor

**Power:**
- L298N 12V input → 6V battery
- Remove ENA/ENB jumpers on L298N
- Connect all grounds together

## How to Use
```cpp
#include "Motor.h"

// Create motor object
Motor leftMotor(26, 27, 25);  // pins: A, B, PWM

void setup() {
    leftMotor.initialize();
}

void loop() {
    leftMotor.forward(80);   // Speed 0-255
    delay(1000);
    leftMotor.stop();
}
```

## Functions

**Control:**
- `setSpeed(speed)` - Set speed (-255 to +255, negative = backward)
- `forward(speed)` - Move forward (0 to 255)
- `backward(speed)` - Move backward (0 to 255)
- `stop()` - Coast to stop (no power)
- `brake()` - Active brake (fast stop)

**Status:**
- `getSpeed()` - Get current speed setting
- `isMoving()` - Check if motor has power

## Speed Guidelines

**For small micromouse (10cm diameter):**
- Start testing: 50-60 (safe)
- Normal operation: 70-90
- Maximum: 100-120
- Avoid: 150+ (too fast, mechanical stress)

## Troubleshooting

**Motor doesn't spin:**
- Check L298N power LED
- Verify battery voltage (should be 6V)
- Check pin connections
- Remove enable jumpers on L298N

**Motor spins wrong direction:**
- Swap motor wires on L298N output
- Or swap pinA and pinB in code

**Motor stutters:**
- Check battery charge
- Verify grounds connected
- Try lower speed first

## Pin Configuration

Set your pins in `pins.h`:
```cpp
#define MOTOR_LEFT_A      26
#define MOTOR_LEFT_B      27
#define MOTOR_LEFT_PWM    25
```