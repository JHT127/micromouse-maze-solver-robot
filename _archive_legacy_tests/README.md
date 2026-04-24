# Legacy Tests Archive

This directory contains historical exploratory and diagnostic tests from early development phases. They are preserved for reference but **not recommended for current use**.

## Why Archive These?

These tests were useful during **initial hardware exploration and debugging** but have been superseded by:
- More comprehensive, organized unit tests in `../test/`
- Better-structured integration tests
- Cleaner code and more consistent commenting

## 📋 What's Inside

### Basic Hardware Tests
| File | Purpose | Status | Replacement |
|------|---------|--------|-------------|
| `blink_test.cpp` | LED blinking (basic GPIO) | Obsolete | N/A (not needed) |
| `check_pins.cpp` | GPIO pin verification | Obsolete | Manual testing |
| `test_left_encoder.cpp` | Single encoder check | Superseded | `Encoders_Motors_test.cpp` |
| `test_mpu_simple.cpp` | Basic gyro test | Superseded | `MPU6050_test.cpp` |
| `test_i2c_scan.cpp` | Generic I2C scanner | Diagnostic | Use PlatformIO I2C scanner |

### Motor & Calibration Tests
| File | Purpose | Status | Replacement |
|------|---------|--------|-------------|
| `calibrate_motors.cpp` | Manual motor speed calibration | Obsolete | PID tuning in code |
| `find_balance.cpp` | Motor speed balancing (manual) | Obsolete | Use `PIDController_test.cpp` |
| `motors_only_test.cpp` | Motor control without feedback | Superseded | `Encoders_Motors_test.cpp` |
| `test_both_together.cpp` | Early motor + encoder test | Superseded | `Encoders_Motors_test.cpp` |
| `test_left_both.cpp` | Single side test | Diagnostic | Manual hardware check |
| `test_left_once.cpp` | Single motor pulse | Diagnostic | N/A |
| `test_right_once.cpp` | Single motor pulse | Diagnostic | N/A |
| `test_right_slow.cpp` | Slow motor test | Diagnostic | N/A |

### Sensor Tests
| File | Purpose | Status | Replacement |
|------|---------|--------|-------------|
| `test_lidars.cpp` | LIDAR initialization | Obsolete | `LIDARSensor_test.cpp` |
| `test_mpu.cpp` | MPU6050 with Adafruit lib | Obsolete | `MPU6050_test.cpp` |

## 🤔 When Might You Need These?

### Reason 1: Hardware Debugging
If your robot isn't responding:
1. First: Run `Encoders_Motors_test.cpp` (organized test)
2. Last resort: Use these diagnostic tests for isolated pin checking

### Reason 2: Hardware Bringup
If you're adding a new sensor type:
- Review similar test to understand initialization pattern
- Then follow the pattern in the organized `../test/` directory

### Reason 3: Learning Development History
- See how the project evolved from simple tests to integrated systems
- Understand debugging methodology used during development

### Reason 4: Fallback Reference
If an organized test is corrupted (unlikely but possible):
- This archive has backup versions of similar functionality
- Cross-reference with organized test to understand what's missing

## ⚠️ Why NOT to Use These

1. **Inconsistent Commenting**: Mixed commenting styles, less readable
2. **Narrow Scope**: Test one thing at a time with no integration
3. **Redundant**: Same functionality in cleaner tests in `../test/`
4. **Debugging Only**: Not meant for validation or learning
5. **Ad-hoc**: File naming and organization is less systematic

## 📝 File Organization in Archive

```
_archive_legacy_tests/
├── blink_test.cpp                  # LED test
├── calibrate_motors.cpp            # Motor tuning
├── check_pins.cpp                  # GPIO verification
├── find_balance.cpp                # Motor balancing
├── motors_only_test.cpp            # Motor control only
├── test_both_together.cpp          # Motor + encoder
├── test_i2c_scan.cpp               # I2C discovery
├── test_left_both.cpp              # Left motor/encoder
├── test_left_encoder.cpp           # Left encoder only
├── test_lidars.cpp                 # LIDAR init
├── test_mpu.cpp                    # Gyro test (Adafruit)
├── test_mpu_simple.cpp             # Basic gyro test
├── test_right_once.cpp             # Right motor pulse
├── test_right_slow.cpp             # Right motor slow
└── README                          # This file
```

## 🔄 How to Use Archive

### If You Need a Diagnostic Test
1. Check the table above for the test that matches your need
2. Copy it to `../test/` directory
3. Update commenting to match current style
4. Rename with clear purpose: `_diagnostic_motor_left.cpp`

### If You're Studying History
1. Compare `test_left_encoder.cpp` (archive) with `Encoders_Motors_test.cpp` (current)
2. See how tests evolved to be more comprehensive
3. Notice the improved commenting style

## 📊 Statistics

| Metric | Value |
|--------|-------|
| Legacy tests in archive | 14 files |
| Duplicate/redundant with new tests | 12 files |
| Useful diagnostic tests | 2 files |
| Obsolete tests | 5 files |

## ✅ Migration Notes

When this project was cleaned:
- ✓ Removed accidental files
- ✓ Fixed placeholder code
- ✓ Consolidated redundant tests
- ✓ Archived legacy exploratory code
- ✓ Reorganized active tests by component layer
- ✓ Created comprehensive documentation

## 🚀 For Future Development

If you add new functionality:
1. **Do NOT** add tests here
2. Create organized tests in `../test/`
3. Follow naming pattern: `ComponentName_test.cpp`
4. Use consistent commenting style
5. Update `../test/README` with new test documentation

---

**Archived**: April 24, 2026
**Reason**: Codebase cleanup and reorganization
**Status**: Reference only - do not use for active development
**Replacement**: See `../test/` directory for current tests
