# PID Controller Component

## What is this?

A generic PID (Proportional-Integral-Derivative) controller for any control task.

**PID adjusts output to minimize error between target and actual values.**

## How PID Works

**Simple explanation:**

You want motor to spin at 0.1 m/s, but it's only going 0.08 m/s.

Error = 0.1 - 0.08 = 0.02 m/s (too slow)

**P term:** "You're 0.02 too slow, increase power!"  
**I term:** "You've been slow for a while, increase more!"  
**D term:** "You're speeding up fast, don't overshoot!"

Output = P + I + D = motor power adjustment

## How to Use
```cpp
#include "PIDController.h"

// Create PID with gains
PIDController speedPID(2.0, 0.5, 0.1);  // Kp, Ki, Kd

void setup() {
    // Set limits
    speedPID.setOutputLimits(0, 255);     // Motor PWM range
    speedPID.setIntegralLimits(-50, 50);  // Prevent windup
}

void loop() {
    float target = 0.1;      // Want 0.1 m/s
    float actual = 0.08;     // Currently 0.08 m/s
    float dt = 0.02;         // 20ms update
    
    float output = speedPID.compute(target, actual, dt);
    motor.setSpeed(output);  // Apply correction
}
```

## PID Gains Explained

**Kp (Proportional):**
- Reacts to current error
- Higher Kp = stronger immediate reaction
- Too high = oscillation
- Start with: 1.0-3.0

**Ki (Integral):**
- Reacts to accumulated error over time
- Eliminates steady-state error
- Too high = overshoot, instability
- Start with: 0.1-1.0

**Kd (Derivative):**
- Reacts to rate of change
- Reduces overshoot
- Too high = noise amplification
- Start with: 0.0-0.5

## Functions

**Setup:**
- `PIDController(kp, ki, kd)` - Create with gains
- `setGains(kp, ki, kd)` - Change gains
- `setOutputLimits(min, max)` - Limit output range
- `setIntegralLimits(min, max)` - Prevent integral windup

**Control:**
- `compute(target, actual, dt)` - Calculate output
- `reset()` - Clear accumulated values

**Debug:**
- `printGains()` - Show current Kp, Ki, Kd
- `getIntegral()` - Check integral value

## Tuning Guide

**Step-by-step tuning:**

1. **Start with P only:** Ki=0, Kd=0
   - Increase Kp until oscillation starts
   - Reduce Kp by 30%

2. **Add I if needed:**
   - Small Ki (0.1-0.5)
   - If steady-state error, increase Ki
   - If overshoot, decrease Ki

3. **Add D if oscillating:**
   - Small Kd (0.1-0.3)
   - Reduces overshoot
   - Too much = jerky response

**Example progression:**
```
Start:  Kp=1.0, Ki=0.0, Kd=0.0  → Test
Better: Kp=2.0, Ki=0.0, Kd=0.0  → Test (oscillates)
Good:   Kp=1.5, Ki=0.3, Kd=0.0  → Test (still error)
Final:  Kp=2.0, Ki=0.5, Kd=0.1  → Perfect!
```

## Common Issues

**Oscillation (wobbling):**
- Kp too high → reduce
- Kd too low → increase slightly

**Slow to reach target:**
- Kp too low → increase
- Ki too low → increase

**Overshoot:**
- Ki too high → reduce
- Kd too low → increase

**Steady-state error (never quite reaches target):**
- Ki too low → increase
- Or Ki = 0 → add some Ki

**Integral windup (accumulated error gets huge):**
- Set integral limits
- Reset PID when changing targets

## Use Cases in Micromouse

**Speed control:**
```cpp
PIDController speedPID(2.0, 0.5, 0.1);
// Target: desired velocity
// Actual: encoder velocity
// Output: motor PWM
```

**Position control:**
```cpp
PIDController positionPID(3.0, 0.1, 0.5);
// Target: desired position
// Actual: state space position
// Output: velocity command
```

**Angle control:**
```cpp
PIDController anglePID(2.5, 0.0, 0.3);
// Target: desired heading
// Actual: gyro angle
// Output: angular velocity
```

## Expected Behavior

**Good PID tuning:**
- Reaches target quickly (1-2 seconds)
- Little to no overshoot
- Minimal oscillation
- Stays at target (no drift)

**Bad PID tuning:**
- Slow to reach target (5+ seconds)
- Large overshoot
- Continuous oscillation
- Drifts away from target

## Safety Notes

**Always:**
- Start with low gains
- Test on blocks (wheels off ground)
- Set output limits
- Set integral limits
- Reset PID when changing tasks

**Never:**
- Start with high gains (Kp > 5)
- Skip output limits
- Ignore integral windup
- Use same PID for different tasks without retuning
```

---

## Testing Instructions

### Hardware Setup

**Same as before:**
1. Robot on blocks (wheels off ground)
2. 6V battery connected to L298N
3. ESP32 powered via USB
4. All grounds connected

### Running Tests

1. Upload code to ESP32
2. Open Serial Monitor (9600 baud)
3. Press reset button
4. Watch 4 tests run automatically

### What You Should See

**Test 1: Single Wheel Speed Control**
```
Target speed: 0.080 m/s
Time: 0.5s | Target: 0.080 m/s | Actual: 0.045 m/s | PWM: 68
Time: 1.0s | Target: 0.080 m/s | Actual: 0.072 m/s | PWM: 82
Time: 1.5s | Target: 0.080 m/s | Actual: 0.078 m/s | PWM: 85
Time: 2.0s | Target: 0.080 m/s | Actual: 0.080 m/s | PWM: 86
Time: 2.5s | Target: 0.080 m/s | Actual: 0.080 m/s | PWM: 86

✓ Single wheel PID works!
```

**Pattern to observe:**
- Actual speed starts low
- PWM increases to compensate
- Speed converges to target
- PWM stabilizes

**Test 2: Both Wheels WITHOUT PID**
```
Time: 0.5s | Left: 0.052 m/s | Right: 0.058 m/s | Position: [x=0.0012m, y=0.0275m, angle=2.15°]
Time: 1.0s | Left: 0.053 m/s | Right: 0.059 m/s | Position: [x=0.0025m, y=0.0556m, angle=4.31°]
Time: 1.5s | Left: 0.052 m/s | Right: 0.058 m/s | Position: [x=0.0039m, y=0.0832m, angle=6.48°]
Time: 2.0s | Left: 0.053 m/s | Right: 0.059 m/s | Position: [x=0.0053m, y=0.1110m, angle=8.63°]
Time: 2.5s | Left: 0.052 m/s | Right: 0.058 m/s | Position: [x=0.0068m, y=0.1387m, angle=10.79°]
Time: 3.0s | Left: 0.053 m/s | Right: 0.059 m/s | Position: [x=0.0083m, y=0.1665m, angle=12.95°]

Final position WITHOUT PID:
Position: [x=0.0083m, y=0.1665m, angle=12.95°]
Notice: Robot curved (X != 0, angle != 0)
```

**Pattern to observe:**
- Left and right speeds DIFFERENT
- X position increases (drifting sideways)
- Angle increases (rotating)
- Robot clearly not going straight

**Test 3: Both Wheels WITH PID**
```
Time: 0.5s | Left: 0.073 m/s | Right: 0.075 m/s | Position: [x=0.0002m, y=0.0370m, angle=0.43°]
Time: 1.0s | Left: 0.079 m/s | Right: 0.080 m/s | Position: [x=0.0003m, y=0.0791m, angle=0.68°]
Time: 1.5s | Left: 0.080 m/s | Right: 0.080 m/s | Position: [x=0.0003m, y=0.1201m, angle=0.72°]
Time: 2.0s | Left: 0.080 m/s | Right: 0.080 m/s | Position: [x=0.0003m, y=0.1601m, angle=0.73°]
Time: 2.5s | Left: 0.080 m/s | Right: 0.080 m/s | Position: [x=0.0003m, y=0.2001m, angle=0.73°]
Time: 3.0s | Left: 0.080 m/s | Right: 0.080 m/s | Position: [x=0.0003m, y=0.2401m, angle=0.73°]

Final position WITH PID:
Position: [x=0.0003m, y=0.2401m, angle=0.73°]
Notice: Straighter line (X ≈ 0, angle ≈ 0)
```

**Pattern to observe:**
- Left and right speeds CONVERGE
- Both reach target (0.080 m/s)
- X position minimal (< 1mm)
- Angle minimal (< 1°)
- Much straighter than Test 2!

**Test 4: Velocity Matching**
```
Left: 0.0523 m/s (PWM: 65) | Right: 0.0568 m/s (PWM: 71) | Diff: -4.50 mm/s
Left: 0.0687 m/s (PWM: 78) | Right: 0.0712 m/s (PWM: 82) | Diff: -2.50 mm/s
Left: 0.0754 m/s (PWM: 83) | Right: 0.0768 m/s (PWM: 85) | Diff: -1.40 mm/s
Left: 0.0782 m/s (PWM: 85) | Right: 0.0791 m/s (PWM: 86) | Diff: -0.90 mm/s
Left: 0.0795 m/s (PWM: 86) | Right: 0.0799 m/s (PWM: 87) | Diff: -0.40 mm/s
Left: 0.0798 m/s (PWM: 86) | Right: 0.0800 m/s (PWM: 87) | Diff: -0.20 mm/s
Left: 0.0800 m/s (PWM: 86) | Right: 0.0800 m/s (PWM: 87) | Diff: 0.00 mm/s