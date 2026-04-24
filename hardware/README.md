# 🔧 Hardware

This directory covers the physical robot — components, wiring, power, and mechanical design.

---

## Bill of Materials (BOM)

| # | Component | Qty | Purpose |
|---|-----------|-----|---------|
| 1 | ESP32 Dev Kit (30-pin) + Shield | 1 | Main microcontroller — dual-core, 34 GPIO |
| 2 | L298N H-bridge Motor Driver | 1 | Drives both DC motors (handles high current) |
| 3 | N20 DC Motor (geared) | 2 | Differential drive locomotion |
| 4 | Wheel Encoder (530 PPR) | 2 | Odometry — measures exact wheel rotation |
| 5 | VL53L0X LIDAR (ToF sensor) | 3 | Wall detection: front, left, right |
| 6 | MPU-6050 IMU | 1 | 3-axis gyroscope + 3-axis accelerometer |
| 7 | Driven wheels | 2 | Forward/backward/turning motion |
| 8 | Caster wheel | 1 | Balance without extra motor |
| 9 | Lithium-ion battery | 1 | Main power source |
| 10 | Buck-Boost converter | 1 | Regulates voltage as battery drains |
| 11 | 3D-printed PLA chassis | 1 | Custom compact body |

---

## Wiring Summary

```
ESP32 GPIO          → Peripheral
─────────────────────────────────
GPIO 21 (SDA)       → All I2C devices (VL53L0X ×3, MPU6050)
GPIO 22 (SCL)       → All I2C devices
GPIO MOTOR_LEFT_A/B → L298N IN1/IN2
GPIO MOTOR_RIGHT_A/B→ L298N IN3/IN4
GPIO MOTOR_LEFT_PWM → L298N ENA (PWM)
GPIO MOTOR_RIGHT_PWM→ L298N ENB (PWM)
GPIO ENCODER_LEFT_A/B  → Left encoder A/B channels
GPIO ENCODER_RIGHT_A/B → Right encoder A/B channels
GPIO LIDAR_*_XSHOT  → VL53L0X XSHUT pins (for I2C address assignment)
GPIO LIDAR_*_INT    → VL53L0X interrupt pins
```

Full pin mapping: [`../software/include/pins.h`](../software/include/pins.h)

---

## I2C Address Assignment

The three VL53L0X sensors share the I2C bus but are assigned unique addresses at startup:

| Sensor | Address | XSHUT pin |
|--------|---------|-----------|
| Front LIDAR | `0x30` | `LIDAR_FRONT_XSHOT` |
| Left LIDAR | `0x31` | `LIDAR_LEFT_XSHOT` |
| Right LIDAR | `0x32` | `LIDAR_RIGHT_XSHOT` |

Sensors are enabled one at a time during boot — see `setup()` in `src/main.cpp`.

---

## Power Architecture

```
Li-ion Battery
      │
  Buck-Boost Converter (regulates output voltage)
      │
      ├──→ L298N (motor power, 7–12V line)
      └──→ ESP32 + Sensors (3.3V / 5V logic)
```

The buck-boost converter maintains stable voltage even as the battery discharges — critical for consistent PID and sensor performance.

---

## 3D Printed Chassis

See [`3d-models/`](3d-models/) for STL files.

**Design rationale:**
- Compact footprint to fit maze cells cleanly
- Lightweight PLA body reduces motor load → better acceleration
- Cardboard prototype was built first to validate dimensions before printing
- Component placement tested on cardboard before committing to print

**Recommended print settings:**
- Material: PLA
- Layer height: 0.2 mm
- Infill: 20%
- Supports: as needed for overhangs

---

## Component Notes

### ESP32 + Shield
The shield provides organized breakout connections and stable power rails. The dual-core processor lets us handle sensor reads and motor control simultaneously without blocking.

### L298N H-bridge
GPIO pins cannot supply enough current to drive motors directly. The L298N acts as a power bridge: it takes low-current direction signals from ESP32 GPIO and PWM signals for speed, and delivers the high-current output the motors need.

### VL53L0X LIDAR (×3)
Uses time-of-flight laser pulses — more reliable than ultrasonic in speed and precision. Three sensors give the robot full left/front/right wall awareness. Range: 50–1200 mm.

### MPU-6050
6-axis IMU. We use the gyroscope to confirm accurate 90° turns and correct wheel-slippage errors that encoders alone would miss. Works in combination with encoders for more reliable heading estimation.

### N20 Motors + Encoders
530 PPR encoders on motor shafts provide tick-level feedback. Used to calculate: exact distance travelled per tick, real-time wheel velocity (fed into PID), and odometry for the StateSpace position model.
