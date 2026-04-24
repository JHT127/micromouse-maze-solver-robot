# 🐭 Micromouse Maze Solver Robot

> An autonomous maze-solving robot built from scratch — custom 3D-printed chassis, embedded C++ firmware, and classical control theory running on an ESP32.

**Team**:  Joud Thaher · Layan Salem · Labiba Sharia · Waad Ziadeh

**Instructor**: Dr. Wasel  

**University**: Birzeit University 

**Robot Name**: SHAMSHOON

---

## 🎯 What This Project Does

The robot navigates an 8×8 cell maze autonomously without any human input. It:

1. **Senses** walls in real time using three LIDAR sensors (front, left, right)
2. **Localises** itself in the maze using wheel encoders + gyroscope (StateSpace model)
3. **Plans** the shortest path to the center using the Flood Fill algorithm (BFS)
4. **Executes** precise movements using dual PID controllers — one per wheel
5. **Updates** its wall map as it explores and re-solves the path dynamically

Everything — from the PCB wiring to the 3D-printed body to the firmware — was designed and built by the team.

---

## 📸 Demo & Documentation

| Resource | Link |
|----------|------|
| 📄 Full Project Presentation | [`docs/presentation.pdf`](docs/presentation.pdf) |
| 🗂️ Project Journey (Trello) | [Trello Board](https://trello.com/b/NDhEVEoG/micromouse-maze-solver-project) |
| 🏗️ Software Architecture | [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) |
| 🔧 Hardware Overview | [`hardware/README.md`](hardware/README.md) |
| 🖨️ 3D Print Files | [`hardware/3d-models/`](hardware/3d-models/) |

---

## 🤖 System Overview

```
┌─────────────────────────────────────────────────────────┐
│                    MazeNavigator                        │  ← Orchestration
├──────────────────────┬──────────────────────────────────┤
│     FloodFill        │       MotionController           │  ← Planning & Control
│  (BFS path solver)   │  (PID + StateSpace + gyro)      │
├──────────────────────┴──────────────────────────────────┤
│  LIDARSensor ×3  │  Encoder ×2  │  MPU6050Driver       │  ← Sensing
├──────────────────┴──────────────┴──────────────────────-┤
│              Motor ×2  │  PIDController ×2              │  ← Actuation
└─────────────────────────────────────────────────────────┘
                       ESP32 (Dual-core)
```

---

## 🛠️ Hardware Stack

| Component | Part | Role |
|-----------|------|------|
| **MCU** | ESP32 Dev Kit + Shield | Brain — dual-core processing, all GPIO |
| **Motors** | 2× N20 DC + L298N H-bridge | Differential drive |
| **Encoders** | 530 PPR wheel encoders | Odometry / speed feedback |
| **LIDAR** | 3× VL53L0X (I2C) | Wall detection: front, left, right |
| **IMU** | MPU6050 (6-axis) | Gyroscope for heading, accel for tilt |
| **Power** | Li-ion battery + Buck-Boost | Stable voltage across charge levels |
| **Chassis** | Custom 3D-printed PLA | Compact, lightweight, maze-optimised |

---

## 💻 Software Stack

| Layer | Module | What it does |
|-------|--------|--------------|
| **Sensing** | `LIDARSensor`, `MPU6050Driver`, `Encoder` | Read raw hardware data |
| **Control** | `PIDController`, `Motor` | Regulate wheel speed via PWM |
| **Localisation** | `StateSpace` | Kinematic model → tracks (x, y, θ) |
| **Motion** | `MotionController` | High-level: move forward, turn 90° |
| **Planning** | `FloodFill` | BFS flood → distance field for 8×8 maze |
| **Integration** | `MazeNavigator` | Ties everything into a solve loop |

**Toolchain**: C++ · Arduino Framework · PlatformIO · VS Code  
**Libraries**: `Adafruit MPU6050`, `Pololu VL53L0X`

---

## 🚀 Quick Start (Software)

### Prerequisites
- [VS Code](https://code.visualstudio.com/) + [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 connected via USB

### Steps

```bash
git clone https://github.com/JHT127/micromouse-maze-solver-robot.git
cd micromouse-maze-solver-robot/software
```

Open in VS Code → PlatformIO will auto-install dependencies.

Edit `software/platformio.ini` to set your port:
```ini
upload_port = COM4       # Windows — change to your port
# upload_port = /dev/ttyUSB0   # Linux
# upload_port = /dev/cu.SLAB_USBtoUART  # macOS
```

```bash
# Build & upload firmware
platformio run -t upload

# Monitor serial output
platformio device monitor --baud 9600
```

> ⚠️ Keep the robot stationary during startup — the IMU calibrates for 500 samples.

---

## 📂 Repository Structure

```
micromouse-maze-solver-robot/
│
├── README.md                   ← You are here
│
├── docs/
│   ├── presentation.pdf        ← Full project presentation
│   ├── ARCHITECTURE.md         ← Deep-dive: algorithms & design decisions
│   └── component-READMEs/     ← Per-module documentation
│       ├── FloodFill.md
│       ├── PIDController.md
│       ├── StateSpace.md
│       └── ...
│
├── hardware/
│   ├── README.md               ← Hardware setup, wiring, BOM
│   └── 3d-models/             ← STL files for chassis 
│   
│
└── software/                   ← All firmware (PlatformIO project)
    ├── platformio.ini
    ├── src/
    │   └── main.cpp            ← Entry point
    ├── lib/                    ← 9 modular libraries
    │   ├── Motor/
    │   ├── Encoder/
    │   ├── PIDController/
    │   ├── StateSpace/
    │   ├── MPU6050Driver/
    │   ├── MotionController/
    │   ├── LIDARSensor/
    │   ├── FloodFill/
    │   └── MazeNavigator/
    ├── include/
    │   └── pins.h              ← All GPIO pin definitions
    ├── test/                   ← 12 unit + integration tests
    └── _archive_legacy_tests/  ← Early exploratory tests (historical)
```

---

## 🧪 Tests

12 tests cover every major component:

| Test | Type | What's validated |
|------|------|-----------------|
| `FloodFill_test` | Unit | BFS algorithm, distance field, wall encoding |
| `PIDController_test` | Unit | Gain tuning, step response |
| `StateSpace_test` | Unit | Position prediction, coordinate transforms |
| `Encoders_Motors_test` | Unit | Speed measurement, PWM output |
| `LIDARSensor_test` | Unit | I2C addresses, ranging |
| `MPU6050_test` | Unit | Init, calibration, gyro drift |
| `MotionController_test` | Integration | Forward + turn commands |
| `turn_90_test` | Integration | Rotation accuracy |
| `MoveForward_WallDetection_test` | Integration | Movement + obstacle avoidance |
| `moveAndTurn_Test` | Integration | Sequential motion patterns |
| `CollisionAvoidance_test` | Integration | Emergency stop |
| `MazeNavigator_test` | Integration | Full maze solve loop |

See [`software/test/README.md`](software/test/README.md) for how to run individual tests.

---

## 🖨️ 3D Printing

The robot chassis was designed in-house and 3D-printed in PLA. Key design decisions:
- **Compact footprint** — fits within maze cell constraints
- **Lightweight** — reduces motor load, improves acceleration
- **Cardboard prototype first** — we validated dimensions before printing

STL files are in [`hardware/3d-models/`](hardware/3d-models/).  
*(If you're replicating this project, we recommend printing at 20% infill with 0.2mm layer height.)*

---

## 📐 Key Algorithms

### Flood Fill (Path Planning)
BFS from the goal (center 4 cells of the 8×8 grid) outward. Each cell stores a distance value and 4-bit wall encoding. The robot always moves to the lowest-distance neighbour — guaranteed shortest path.

### PID Control (Speed Regulation)
One PID per wheel. Input: target velocity. Feedback: encoder ticks. Output: PWM (0–255). Update rate: 50 Hz. Gains tuned manually: start with P, add I to remove steady-state error, add D to reduce oscillation.

### State Space (Localisation)
Kinematic differential-drive model tracking `[x, y, θ]`. Linear velocity from encoders, angular velocity from gyroscope. Heading convention: 0° = North, 90° = East, clockwise positive.

---

## 🔐 Security Note

This repository contains **no credentials, API keys, or Wi-Fi passwords**. The `platformio.ini` file contains a COM port reference (`COM4`) — update this to match your machine before uploading. No other machine-specific configuration is hardcoded.

---

## 📄 License

This project is submitted as academic coursework. You are welcome to study, fork, and learn from the code. Please note that we are students and still learning, so double-check everything. Do not forget to check out our journey documented on trello (the link is provided above if you click on the words Trello Board)! 
