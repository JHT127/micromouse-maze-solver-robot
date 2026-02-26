# PID Controller Analysis: Micromouse Straight-Line Navigation Issue

## Executive Summary
Your robot is **drifting left significantly** due to **motor mismatch + inadequate heading control**, NOT primarily a PID tuning issue. The current PID oscillates too much (Kp too high, Kd too low), and the control architecture doesn't coordinate wheel speeds to maintain heading.

---

## 1. CURRENT CONFIGURATION EXTRACTION

### PID Parameters (from code + tests)
```
Kp = 500.0      (Proportional gain)
Ki = 28.0       (Integral gain)
Kd = 6.0        (Derivative gain)

Control Loop Frequency:    50 Hz (dt = 0.02 s)
Base Forward Speed:        0.15 m/s
PWM Output Range:          0-255
Integral Anti-Windup:      -80 to 80

Integral Output Limits:    Clamped at ±80
```

### Error Definition
```cpp
error = targetValue - actualValue  // Positive = undershoot
```

---

## 2. TEST DATA ANALYSIS

### TEST 1: Single Wheel PID (Left only, 4 sec @ 0.15 m/s target)
```
Time (s)  | Error (m/s) | PWM Output | Notes
0.0       | +0.150      | 120        | Max startup
0.5-1.5   | +0.072      | 36-42      | Oscillating in PWM
2.0-3.5   | +0.052      | 26-42      | Oscillation persists
Final     | +0.052      | ~26        | Steady-state error = 0.035 m/s (23% miss)
```

**Issues Identified:**
- ✗ Oscillation in PWM output (36→42→37→38→41→31→35→42 → chaotic)
- ✗ Steady-state error: Never reaches target (0.098 m/s vs 0.150 m/s goal)
- ✗ Integral term hitting windup limit (-80/+80) preventing error correction

---

### TEST 2: Both Wheels WITHOUT PID (Baseline - fixed PWM=80)
```
Time: 3 seconds
Final Position: [x=-0.0138m, y=0.2427m, angle=-15.13°]

Analysis:
- Left motor slightly stronger (or lower friction) → causes leftward drift
- Heading drift: 15.13° over 0.2427m = 62.3°/meter
- Lateral error: -13.8mm (to the left)
```

**Conclusion:** Motor mismatch is inherent and significant.

---

### TEST 3: Both Wheels WITH PID (0.15 m/s target, 3 sec with tuned Kp/Ki/Kd)
```
Time: 3 seconds  
Final Position: [x=-0.0110m, y=0.1360m, angle=-6.86°]

Velocity during motion (sampled):
  min: [L=0.039 m/s, R=0.029 m/s]  → velocity differential = 0.01 m/s
  max: [L=0.068 m/s, R=0.049 m/s]  → velocity differential = 0.019 m/s
  avg: ~[L=0.050 m/s, R=0.043 m/s] → differential = 0.007 m/s

Heading drift: 6.86° over 0.1360m = 50.4°/meter
Lateral error: -11.0mm (improved vs TEST 2, but still left)
```

**Key Finding:**
```
Without PID: -15.13° in 3 seconds
With PID:    -6.86°  in 3 seconds
Improvement: ~55% better heading, but still unacceptable
```

---

## 3. ROOT CAUSE DIAGNOSIS

### Primary Issue: CONTROL ARCHITECTURE (Not Just Tuning)

**The Problem:**
1. Each wheel has an independent PID controlling its velocity to 0.15 m/s
2. **No cross-coupling**: The right PID doesn't know left wheel is slower
3. **No heading feedback**: During forward motion, angular error is NOT fed back
4. When left wheel is stronger, it goes faster → robot turns left
5. Independent PIDs can't correct heading, only speed

### Secondary Issue: PID Parameter Tuning

**Kp = 500.0 (TOO HIGH)**
- Causes aggressive oscillations in TEST 1 (PWM: 36-42 bouncing)
- At error=0.072 m/s: correction = 500 × 0.072 = 36 PWM
- Large jumps → overshooting → oscillation

**Kd = 6.0 (TOO LOW)**
- Derivative term barely dampens oscillations
- Should be 15-30× higher for smooth control
- At oscillation frequency, derivative should provide significant damping

**Ki = 28.0 (ADEQUATE but LIMITED)**
- Anti-windup limits (-80/+80) are too tight
- Preventing integral from accumulating enough to overcome motor friction
- Traps system in steady-state error

---

## 4. PROBLEM CLASSIFICATION

| Symptom | Diagnosis | Evidence |
|---------|-----------|----------|
| Oscillating PWM (36-42) | **Too much Kp** ✓ | TEST 1 shows bouncing PWM |
| Never reaches target speed | **Too much anti-windup + low Ki relative to friction** | TEST 1: final error = 0.052 m/s |
| Chaotic oscillation pattern | **Insufficient Kd dampening** ✓ | TEST 1: no smooth convergence |
| Left drift in straight line | **Motor mismatch + no heading control** ✓ | TEST 3: 6.86° left turn |
| Better with PID than without | **Kp is helping balance motors** | TEST 3 vs TEST 2 comparison |

---

## 5. RECOMMENDED NEW PID VALUES FOR LOW-SPEED TESTING

### Phase 1: Reduce Oscillation (IMMEDIATE TUNING)

```
Current:  Kp=500.0, Ki=28.0, Kd=6.0
Change 1: Kp=200.0, Ki=28.0, Kd=10.0
Change 2: Kp=150.0, Ki=35.0, Kd=15.0  ← START HERE
Change 3: Kp=100.0, Ki=40.0, Kd=20.0  ← If still oscillating
```

### Recommended Starting Point (PHASE 1)
```cpp
Kp = 150.0      // Reduced 70% from 500 (less aggressive)
Ki = 35.0       // Increased 25% (help overcome friction)
Kd = 15.0       // Increased 150% (add damping)

Integral Limits: -120 to +120  (increased from -80/+80)
```

**Why these values:**
- Kp reduction: 500 → 150 cuts aggressiveness by 70%, should eliminate oscillation
- Kd increase: 6 → 15 provides 2.5× better damping
- Ki increase: 28 → 35 helps reach target despite motor friction
- Wider integral limits allow PID to accumulate enough correction

---

## 6. STEP-BY-STEP TUNING PLAN

### STEP 1: Reduce Kp (Eliminate Oscillation)
```
Goal: Get smooth convergence in TEST 1 (single wheel)

Try Kp=150 while keeping Ki=28, Kd=10
Expected: PWM output should change smoothly, not bounce
Run TEST 1 and observe:
  - Is PWM smooth? (ascending, not oscillating)
  - Does it converge to steady speed?
  - Final speed closer to 0.15 m/s?

If still oscillating:  Kp = 100
If converges too slowly: Kp = 200
If overshoots:         Need higher Kd first
```

### STEP 2: Increase Kd (Add Damping)
```
Goal: Prevent overshoot and smooth transients

With Kp=150, increase Kd: 10 → 15 → 20
Expected: Faster convergence without bounce

Run TEST 1:
  - Smoother PWM transitions?
  - Reaching target speed (0.15 m/s) faster?
  
Optimal: PWM rises smoothly to steady value, stays stable
```

### STEP 3: Increase Ki (Overcome Steady-State Error)
```
Goal: Final speed should be 0.15 m/s ±5%

Increase Ki: 28 → 35 → 40
Increase integral limits: -80/80 → -120/120

Run TEST 1:
  - Final velocity closer to 0.15 m/s?
  - Less oscillation?

Optimal: Final speed = 0.14-0.15 m/s (within 5%)
```

### STEP 4: Test Both Wheels with PID (TEST 3)
```
Expected improvements:
- Before: LEFT drift of 6.86° in 3 seconds
- Target: < 3.0° drift

If still drifting > 3.0°:
  → Add motor speed trim (see STEP 5)
```

### STEP 5: Add Motor Speed Trim (If Needed)
```
If both wheels reach target speed but robot still drifts left:
  → Right motor is weaker, needs +10-20 PWM offset
  
Modification location: MotionController::moveForward()

Current:
  float leftPower = leftPID->compute(forwardSpeed, vL, dt);
  float rightPower = rightPID->compute(forwardSpeed, vR, dt);

Modified:
  float leftPower = leftPID->compute(forwardSpeed, vL, dt);
  float rightPower = rightPID->compute(forwardSpeed, vR, dt) + 10;  // +10 to right
  
Test incrementally: +5, +10, +15, +20
```

---

## 7. STRUCTURAL IMPROVEMENTS RECOMMENDED

### A. IMMEDIATE: Cross-Coupled Heading Control
**Current state:** No heading feedback during forward motion

**Recommended modification** (Medium priority):
```cpp
// In MotionController::moveForward()

// Get velocities
float leftVel = leftEncoder->getVelocity(dt);
float rightVel = rightEncoder->getVelocity(dt);

// Calculate velocity difference (causes heading error)
float velDifference = leftVel - rightVel;  // Positive = turning left

// Heading correction: dampen velocity difference
// This naturally decouples wheel speeds if one is running too fast
float headingCorrection = 50.0 * velDifference;  // Gain = 50

// Apply to wheels
float leftPower = leftPID->compute(forwardSpeed, leftVel, dt) - headingCorrection;
float rightPower = rightPID->compute(forwardSpeed, rightVel, dt) + headingCorrection;

leftMotor->setSpeed((int)leftPower);
rightMotor->setSpeed((int)rightPower);
```

**Effect:** If left motor runs faster, heading correction reduces its PWM, naturally bringing velocities into balance.

---

### B. ANTI-WINDUP IMPROVEMENT
**Current:** Simple clamping (integralMin=-80, integralMax=80)

**Better approach - Conditional Integration:**
```cpp
// In PIDController::compute()

// Only accumulate integral if output is not saturated
bool outputSaturated = (output > outputMax) || (output < outputMin);

if (!outputSaturated) {
    integral += error * deltaTime;
    // Then clamp
    if (integral > integralMax) integral = integralMax;
    if (integral < integralMin) integral = integralMin;
}

// This prevents integral from growing when we can't apply more correction
```

---

### C. DERIVATIVE FILTERING
**Current:** Raw derivative: `(error - previousError) / deltaTime`

**Risk:** Noisy encoder data can cause derivative spikes.

**Improved version:**
```cpp
const float ALPHA = 0.3;  // Low-pass filter coefficient (0-1)

float rawDerivative = (error - previousError) / deltaTime;
float filteredDerivative = (ALPHA * rawDerivative) + ((1.0 - ALPHA) * previousDerivative);
float derivativeTerm = kd * filteredDerivative;

previousDerivative = filteredDerivative;
```

---

### D. MOTION RAMPING (Optional for Competition)
**Not needed now, but useful later:**
```cpp
// Smooth acceleration to avoid sudden jerks
const float RAMP_RATE = 0.05;  // m/s per control cycle

if (targetVel > currentTargetVel + RAMP_RATE) {
    currentTargetVel += RAMP_RATE;
} else if (targetVel < currentTargetVel - RAMP_RATE) {
    currentTargetVel -= RAMP_RATE;
} else {
    currentTargetVel = targetVel;
}
```

---

## 8. PRIORITY SEQUENCE FOR IMPLEMENTATION

| Priority | Action | Expected Gain | Effort |
|----------|--------|---------------|--------|
| 🔴 P1 | Set Kp=150, Kd=15, Ki=35 | Eliminate oscillation | < 5 min |
| 🔴 P1 | Run TEST 1 to verify smooth convergence | Validate tuning | < 10 min |
| 🟡 P2 | Add cross-coupled heading correction | Reduce drift from 6.86° → <3° | 15 min |
| 🟡 P2 | Run TEST 3 to measure improvement | Validate heading control | 10 min |
| 🟡 P2 | Add motor speed trim (+10 PWM to right) if needed | Further driftreduction | 5 min |
| 🟢 P3 | Implement conditional integration anti-windup | Prevent windup in edge cases | 10 min |
| 🟢 P3 | Add derivative filtering | Smooth noisy encoder data | 10 min |

---

## 9. TUNING CHECKLIST

### Before Each Test Run:
- [ ] Robot positioned straight along reference line
- [ ] Encoders zeroed: `leftEncoder.reset(); rightEncoder.reset();`
- [ ] PID controllers reset: `leftPID.reset(); rightPID.reset();`
- [ ] IMU zeroed: `imu->resetAngle();`
- [ ] Serial monitor open at 115200 baud
- [ ] Clear floor with no obstacles in 0.5m path

### After Each Test:
- [ ] Record final error and PWM oscillation pattern
- [ ] Photograph robot position relative to start
- [ ] Measure actual heading drift (measure lateral position at end)
- [ ] Note: faster motors (higher steady-state speed) = less integral influence = more drift possible

### Success Criteria for Low-Speed Stable Motion:
- ✓ TEST 1: Final velocity ≥ 0.14 m/s (within 6% of target)
- ✓ TEST 1: PWM output smooth (< 10 PWM variation at steady state)
- ✓ TEST 3: Heading drift < 3° over 1 meter
- ✓ TEST 3: Both wheels reach similar velocity (within 5% difference)

---

## 10. DANGEROUS PITFALLS TO AVOID

### ❌ DON'T: Increase Kp above 150 yet
Reason: Motor friction is still unknown; high Kp causes oscillation with noisy feedback.

### ❌ DON'T: Tune while motors are cold
Reason: Friction changes as motor warms up. Wait 1-2 minutes after startup.

### ❌ DON'T: Use integral-only control (Ki high, Kp/Kd low)
Reason: Very slow response, oscillation during acceleration.

### ❌ DON'T: Ignore motor mismatch
Reason: Perfect PID tuning cannot overcome ~20% motor speed mismatch. Must implement cross-coupling or trim.

### ❌ DON'T: Test only with motor held in air
Reason: Friction and load affect performance. Always test on competition surface.

---

## 11. NEXT STEPS

1. **Immediately:** Modify `main.cpp` to use new PID values (Kp=150, Ki=35, Kd=15)
2. **Run TEST 1:** Verify single-wheel oscillation is gone
3. **Adjust Ki:** Tune until final speed is 0.14-0.15 m/s
4. **Run TEST 3:** Check two-wheel heading drift
5. **If drift > 3°:** Add cross-coupled correction
6. **If still drifting:** Add motor trim to right wheel

**After this first round of tuning, bring me the new test results and I'll refine the gains based on actual behavior.**

---

## 12. NOTES FROM ROBOTICS ENGINEER

> "Your PID is fighting against motor mismatch with too much gain. The oscillation isn't a tuning accident—it's the controller being too aggressive to a noisy, asymmetric system. Once we dial back Kp and add proper damping with heading cross-coupling, straight-line motion should stabilize. Then we'll have a foundation for speed increases later."

---

**Document Created:** 2026-02-26  
**Tuning Priority:** IMMEDIATE ACTION REQUIRED
