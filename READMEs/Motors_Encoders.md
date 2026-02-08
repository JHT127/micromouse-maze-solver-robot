# Micromouse - Motors and Encoders

## What is this?

Integration of motor control and encoder feedback with state space model.

**Components:**
- Motor class: Controls L298N motor driver
- Encoder class: Reads N20 motor encoders
- StateSpace class: Tracks position

## Hardware Setup

### Motor Connections (L298N)
- Left motor: pins 26, 27 (direction), 25 (PWM)
- Right motor: pins 4, 13 (direction), 14 (PWM)

### Encoder Connections
- Left encoder: pins 32, 33
- Right encoder: pins 34, 35

### Power
- Motors: External 6V battery to L298N
- ESP32: USB or separate 5V supply
- **IMPORTANT**: Connect grounds together!

## How to Test

1. **Upload code** to ESP32
2. **Open Serial Monitor** (9600 baud)
3. **Place robot on blocks** (wheels shouldn't touch ground)
4. **Press reset** and watch tests

## What Tests Do

**Test 1:** Individual motor control
- Each motor spins separately
- Encoders count ticks

**Test 2:** Both motors together
- Forward motion
- Tick counts should be similar

**Test 3:** Velocity measurement
- Motors at constant speed
- Velocity should stabilize around 0.08-0.12 m/s

**Test 4:** State space tracking
- Position updates based on encoder feedback
- Should show robot "moving" forward

## Expected Results
```
Test 1: Each motor spins, encoder counts increase
Test 2: Both motors spin, similar tick counts
Test 3: Velocities stabilize at ~0.1 m/s
Test 4: State space Y position increases to ~0.3m
```

## Troubleshooting

**Motor doesn't spin:**
- Check power connections
- Check L298N enable jumpers
- Verify pin numbers in pins.h

**Encoder doesn't count:**
- Check encoder power (usually 3.3V or 5V)
- Verify pin connections
- Test with multimeter (should see pulses)

**Counts only on one encoder:**
- Check other encoder connections
- Verify interrupt is attached

**Robot goes in circles:**
- Normal! We'll fix with PID next
- Motors have different speeds naturally

## Next Steps

Now that motors and encoders work:
1. Day 3: Add PID controller (make wheels same speed)
2. Day 4: Add MPU6050 (accurate turning)
3. Day 5: Add motion primitives (move forward, turn)

## Status

✅ State space model working  
✅ Motor control working  
✅ Encoder reading working  
✅ Integration complete  
➡️ Ready for PID controller
```

---

## Testing Instructions

### Hardware Setup

**Before uploading:**

1. **Power connections:**
   - Connect 6V battery to L298N power input
   - Connect ESP32 USB for programming
   - **Connect all grounds together** (battery, L298N, ESP32)

2. **Motor connections:**
   - Left motor to L298N output A
   - Right motor to L298N output B
   - Verify motor wires (swap if motors spin wrong direction)

3. **Encoder connections:**
   - Left encoder: C1→pin 32, C2→pin 33, VCC→3.3V, GND→GND
   - Right encoder: C1→pin 34, C2→pin 35, VCC→3.3V, GND→GND

4. **Safety:**
   - **Put robot on blocks** so wheels don't touch ground
   - Motors will spin during tests!

### Running Tests

1. Upload code
2. Open Serial Monitor (9600 baud)
3. Press reset on ESP32
4. Watch the 4 tests run automatically

### What You Should See

**Test 1: Basic Motor Control**
```
Left motor forward...
Left encoder ticks: 850
✓ Left motor works

Right motor forward...
Right encoder ticks: 830
✓ Right motor works
```

**Test 2: Both Motors Together**
```
Moving forward for 2 seconds...
Left: 150 ticks | Right: 145 ticks
Left: 320 ticks | Right: 315 ticks
...
✓ Both motors work together
```

**Test 3: Velocity Calculation**
```
Left: 0.0985 m/s | Right: 0.0978 m/s
Left: 0.1002 m/s | Right: 0.0995 m/s
...
✓ Velocity calculation works
```

**Test 4: State Space Integration**
```
Time: 0.5s | Position: [x=0.0001m, y=0.0523m, angle=0.34°]
Time: 1.0s | Position: [x=0.0002m, y=0.1047m, angle=0.68°]
Time: 1.5s | Position: [x=0.0003m, y=0.1571m, angle=1.02°]
...

FINAL STATE:
Position: [x=0.0005m, y=0.2985m, angle=1.85°]
Total distance (encoders): 0.2987 meters

✓ State space tracks position!
```

### Common Issues

**Issue:** Motors don't spin
- **Check:** L298N power LED on?
- **Check:** Remove enable jumpers, connect to PWM pins
- **Check:** Battery charged?

**Issue:** Encoders show 0 ticks
- **Check:** Encoder power connected?
- **Check:** Try swapping C1/C2 wires
- **Check:** Encoder voltage (3.3V or 5V?)

**Issue:** Robot curves/spins
- **This is NORMAL!** Motors have different speeds
- **Fix:** Coming in Day 3 (PID controller)

**Issue:** One encoder much higher than other
- **Check:** Motors spinning same direction?
- **Check:** Encoder mounted correctly?
- **May need:** Swap motor wires to reverse direction

---

## What You Have Now

**Complete motor and encoder system:**

✅ Motor class controlling L298N driver  
✅ Encoder class counting ticks with interrupts  
✅ Velocity calculation from encoders  
✅ State space integration  
✅ All components tested together  
✅ **Ready for PID controller (Day 3)**

**The foundation is solid:**
- Motors respond to commands
- Encoders accurately measure motion
- State space tracks position
- Everything communicates properly

**Next step:** PID controller will make both wheels spin at the **exact same speed** so robot goes straight instead of curving.

---

## How Everything Connects
```
main.cpp
   ↓
   ├─→ Motor (controls L298N)
   │     ↓
   │   Wheels spin
   │     ↓
   ├─→ Encoder (counts ticks)
   │     ↓
   │   Velocity calculated
   │     ↓
   └─→ StateSpace (predicts position)
         ↓
       Position tracked

Next: PID will close the loop
  (adjust motor speed to match target)