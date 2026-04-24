# Encoder Component

## What is this?

Reads wheel rotation from N20 motor magnetic encoders using interrupts.

## Hardware Connections

**N20 Encoder Pins:**
- C1 (Channel A) → ESP32 pin (main counting pin)
- C2 (Channel B) → ESP32 pin (direction detection)
- VCC → 3.3V or 5V (check your encoder specs)
- GND → Ground

**Important:** Connect VCC to correct voltage (usually 3.3V for ESP32)

## How to Use
```cpp
#include "Encoder.h"

// Create encoder objects
Encoder leftEncoder(32, 33, true);   // pins A, B, isLeft
Encoder rightEncoder(34, 35, false); // pins A, B, isRight

void setup() {
    leftEncoder.initialize();
    rightEncoder.initialize();
}

void loop() {
    long ticks = leftEncoder.getTicks();
    float distance = leftEncoder.getDistance();
    float velocity = leftEncoder.getVelocity(0.02);  // dt in seconds
}
```

## Functions

**Reading:**
- `getTicks()` - Get total tick count
- `getDistance()` - Get distance traveled (meters)
- `getVelocity(deltaTime)` - Get speed (m/s)

**Utility:**
- `reset()` - Zero all counters

## Encoder Specifications

**N20 Motor Encoder (530 PPR):**
- Ticks per revolution: 530
- Wheel diameter: 33mm (0.033m)
- Meters per tick: 0.000196m

## How It Works

**Interrupt-based counting:**
1. Hardware detects encoder pulse
2. ISR increments tick counter (very fast)
3. No polling needed (accurate)

**Velocity calculation:**
- Measures ticks between updates
- Converts to distance: ticks × meters_per_tick
- Divides by time: distance / deltaTime

## Troubleshooting

**Encoder shows 0 ticks:**
- Check encoder power connection
- Verify VCC voltage (3.3V or 5V)
- Check C1/C2 pin connections
- Rotate wheel manually, should see counts

**Counts only when motor spins one direction:**
- This is normal for single-channel counting
- We use C1 only for now
- C2 reserved for future direction detection

**Erratic counts:**
- Check wire connections (loose wires)
- Verify encoder mounted securely
- Check for electrical noise (move wires away from motors)

**Very high counts:**
- Normal if wheel spins fast
- 530 ticks per revolution is correct
- Use velocity instead of raw ticks for speed

## Pin Configuration

Set your pins in `pins.h`:
```cpp
#define ENCODER_LEFT_A    32
#define ENCODER_LEFT_B    33
#define ENCODER_RIGHT_A   34
#define ENCODER_RIGHT_B   35
```

## Expected Values

**At moderate speed (PWM ~80):**
- Velocity: 0.05-0.08 m/s
- Ticks per second: ~250-400
- Distance in 1 second: ~0.05-0.08m

**Sanity check:**
- If velocity > 0.5 m/s → Something wrong
- If ticks < 10 in 1 second → Check connections