#include "LIDARSensor.h"

//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Initialize with pins
//------------------------------------------------------------
LIDARSensor::LIDARSensor(int xshut, int interrupt)
    : xshutPin(xshut), intPin(interrupt)
{
    i2cAddress = DEFAULT_ADDRESS;
    isInitialized = false;
    isContinuous = false;
    lastReading = 0;
    
    pinMode(xshutPin, OUTPUT);
    pinMode(intPin, INPUT);
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Set unique I2C address and start continuous mode
//------------------------------------------------------------
bool LIDARSensor::initialize(uint8_t newAddress) {
    // Sensor should already be enabled
    delay(10);
    
    // Initialize sensor at default address
    if (!sensor.init()) {
        Serial.println("LIDAR sensor.init() failed");
        return false;
    }
    
    // Set to new address
    sensor.setAddress(newAddress);
    i2cAddress = newAddress;
    
    // Configure for maze navigation
    sensor.setMeasurementTimingBudget(20000);
    
    isInitialized = true;
    isContinuous = false;
    
    return true;
}

//------------------------------------------------------------
// GET DISTANCE MM
// Purpose: Read distance in millimeters
//------------------------------------------------------------
uint16_t LIDARSensor::getDistanceMM() {
    lastReading = readRangeSingleMillimeters();
    return lastReading;
}

//------------------------------------------------------------
// GET DISTANCE METERS
// Purpose: Read distance in meters
//------------------------------------------------------------
float LIDARSensor::getDistanceMeters() {
    return getDistanceMM() / 1000.0;
}

//------------------------------------------------------------
// IS WALL DETECTED
// Purpose: Check if wall is close
//------------------------------------------------------------
bool LIDARSensor::isWallDetected() {
    float distance;
    if (isContinuous) {
        distance = readRangeContinuousMillimeters() / 1000.0;  // Use continuous reading
    } else {
        distance = getDistanceMeters();  // Use single-shot reading
    }
    return distance < WALL_THRESHOLD;
}

//------------------------------------------------------------
// ENABLE
// Purpose: Set XSHUT high to enable sensor
//------------------------------------------------------------
void LIDARSensor::enable() {
    digitalWrite(xshutPin, HIGH);
}

//------------------------------------------------------------
// DISABLE
// Purpose: Set XSHUT low to disable sensor
//------------------------------------------------------------
void LIDARSensor::disable() {
    digitalWrite(xshutPin, LOW);
}

//------------------------------------------------------------
// IS READY
// Purpose: Check if initialized
//------------------------------------------------------------
bool LIDARSensor::isReady() {
    return isInitialized;
}

//------------------------------------------------------------
// WRITE REGISTER
// Purpose: I2C write to register
//------------------------------------------------------------
void LIDARSensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

//------------------------------------------------------------
// READ REGISTER
// Purpose: I2C read from register
//------------------------------------------------------------
uint8_t LIDARSensor::readRegister(uint8_t reg) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((int)i2cAddress, 1);
    
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

//------------------------------------------------------------
// READ RANGE SINGLE MILLIMETERS
// Purpose: Trigger and read range measurement
//------------------------------------------------------------
uint16_t LIDARSensor::readRangeSingleMillimeters() {
    // Start measurement
    writeRegister(REG_SYSRANGE_START, 0x01);
    
    // Wait for measurement to complete
    uint8_t status = 0;
    unsigned long timeout = millis() + 1000;  // 1 second timeout
    
    while ((status & 0x01) == 0) {
        if (millis() > timeout) {
            Serial.println("LIDAR measurement timeout");
            return 8191;  // Max value (error)
        }
        status = readRegister(REG_RESULT_RANGE_STATUS);
        delayMicroseconds(100);
    }
    
    // Read range result
    Wire.beginTransmission(i2cAddress);
    Wire.write(REG_RESULT_RANGE_STATUS + 1);  // RESULT_RANGE register
    Wire.endTransmission(false);
    Wire.requestFrom((int)i2cAddress, 2);
    
    uint16_t range = 0;
    if (Wire.available() >= 2) {
        range = (Wire.read() << 8) | Wire.read();
    }
    
    return range;
}




//------------------------------------------------------------
// START CONTINUOUS
// Purpose: Start continuous ranging mode (faster readings)
//------------------------------------------------------------
void LIDARSensor::startContinuous() {
    if (!isInitialized) return;
    
    sensor.startContinuous();
    isContinuous = true;
}




//------------------------------------------------------------
// STOP CONTINUOUS
// Purpose: Stop continuous ranging mode
//------------------------------------------------------------
void LIDARSensor::stopContinuous() {
    sensor.stopContinuous();
    isContinuous = false;
}




//------------------------------------------------------------
// READ RANGE CONTINUOUS MILLIMETERS
// Purpose: Get distance in continuous mode (using library)
//------------------------------------------------------------
uint16_t LIDARSensor::readRangeContinuousMillimeters() {
    if (!isInitialized || !isContinuous) {
        return 0;
    }
    
    uint16_t range = sensor.readRangeContinuousMillimeters();
    lastReading = range;
    return range;
}
