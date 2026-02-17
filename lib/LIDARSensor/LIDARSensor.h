#ifndef LIDARSENSOR_H
#define LIDARSENSOR_H

#include <Arduino.h>
#include <Wire.h>

//------------------------------------------------------------
// LIDAR SENSOR CLASS
// Purpose: Read VL53L0X distance sensor
//------------------------------------------------------------

class LIDARSensor {
private:
    //------------------------------------------------------------
    // I2C CONFIGURATION
    // Purpose: Sensor addressing
    //------------------------------------------------------------
    uint8_t i2cAddress;
    int xshutPin;           // Pin to enable/disable sensor
    int intPin;             // Interrupt pin (not used yet)
    
    //------------------------------------------------------------
    // DEFAULT I2C ADDRESS
    // Purpose: All VL53L0X start at this address
    //------------------------------------------------------------
    static const uint8_t DEFAULT_ADDRESS = 0x29;
    
    //------------------------------------------------------------
    // VL53L0X REGISTERS
    // Purpose: Sensor communication
    //------------------------------------------------------------
    static const uint8_t REG_IDENTIFICATION_MODEL_ID = 0xC0;
    static const uint8_t REG_SYSRANGE_START = 0x00;
    static const uint8_t REG_RESULT_RANGE_STATUS = 0x14;
    
    //------------------------------------------------------------
    // WALL DETECTION
    // Purpose: Threshold for wall presence
    //------------------------------------------------------------
    static constexpr float WALL_THRESHOLD = 0.15;  // 15cm
    
    //------------------------------------------------------------
    // STATE
    // Purpose: Track sensor status
    //------------------------------------------------------------
    bool isInitialized;
    uint16_t lastReading;   // mm
    
    //------------------------------------------------------------
    // HELPER FUNCTIONS
    // Purpose: I2C communication
    //------------------------------------------------------------
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    uint16_t readRangeSingleMillimeters();

public:
    //------------------------------------------------------------
    // CONSTRUCTOR
    // Purpose: Set pins
    //------------------------------------------------------------
    LIDARSensor(int xshut, int interrupt);
    
    //------------------------------------------------------------
    // SETUP
    // Purpose: Initialize with unique address
    //------------------------------------------------------------
    bool initialize(uint8_t newAddress);  // Sequential init to avoid conflicts
    
    //------------------------------------------------------------
    // READING FUNCTIONS
    // Purpose: Get distance
    //------------------------------------------------------------
    uint16_t getDistanceMM();           // Distance in millimeters
    float getDistanceMeters();          // Distance in meters
    bool isWallDetected();              // True if distance < threshold
    
    //------------------------------------------------------------
    // UTILITY
    // Purpose: Sensor control
    //------------------------------------------------------------
    void enable();                      // Set XSHUT high
    void disable();                     // Set XSHUT low
    bool isReady();
};

#endif
