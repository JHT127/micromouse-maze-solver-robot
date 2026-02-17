#ifndef MPU6050DRIVER_H
#define MPU6050DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <cmath>

//------------------------------------------------------------
// MPU6050 DRIVER CLASS
// Purpose: Read gyroscope for angle tracking
//------------------------------------------------------------

class MPU6050Driver {
private:
    //------------------------------------------------------------
    // I2C CONFIGURATION
    // Purpose: MPU6050 communication settings
    //------------------------------------------------------------
    static const uint8_t MPU6050_ADDRESS = 0x68;
    static const uint8_t MPU6050_WHO_AM_I = 0x75;
    static const uint8_t MPU6050_PWR_MGMT_1 = 0x6B;
    static const uint8_t MPU6050_GYRO_CONFIG = 0x1B;
    static const uint8_t MPU6050_GYRO_ZOUT_H = 0x47;
    
    //------------------------------------------------------------
    // CALIBRATION
    // Purpose: Store gyro bias for accuracy
    //------------------------------------------------------------
    float gyroBiasZ;        // Z-axis bias (rad/s)
    bool isCalibrated;
    
    //------------------------------------------------------------
    // STATE TRACKING
    // Purpose: Track current angle
    //------------------------------------------------------------
    float currentAngle;     // Integrated angle in radians
    unsigned long lastUpdateTime;
    
    //------------------------------------------------------------
    // CONVERSION CONSTANTS
    // Purpose: Raw data to rad/s conversion
    //------------------------------------------------------------
    static constexpr double GYRO_SCALE = 131.0;
    static constexpr double DEG_TO_RAD = 0.0174532925;
    
    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: I2C communication
    //------------------------------------------------------------
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    int16_t readGyroZ();

public:
    //------------------------------------------------------------
    // SETUP
    // Purpose: Initialize sensor
    //------------------------------------------------------------
    MPU6050Driver();
    bool initialize();              // Returns true if sensor found
    bool calibrate(int samples);    // Measure bias, returns true if success
    
    //------------------------------------------------------------
    // READING FUNCTIONS
    // Purpose: Get angle data
    //------------------------------------------------------------
    void update(float dt);          // Integrate gyro reading
    float getAngularVelocity();     // Current rotation speed (rad/s)
    float getAngle();               // Integrated angle (radians)
    float getAngleDegrees();        // Integrated angle (degrees)
    
    //------------------------------------------------------------
    // UTILITY
    // Purpose: Reset and status
    //------------------------------------------------------------
    void resetAngle();
    void setAngle(float angle);     // Manually set angle (radians)
    bool isReady();                 // Check if calibrated
};

#endif
