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
    lastReading = 0;
    
    pinMode(xshutPin, OUTPUT);
    pinMode(intPin, INPUT);
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Set unique I2C address
//------------------------------------------------------------
bool LIDARSensor::initialize(uint8_t newAddress) {
    // Sensor should already be enabled
    delay(10);
    
    // Read who am i to verify communication at default address
    uint8_t whoAmI = readRegister(REG_IDENTIFICATION_MODEL_ID);
    
    if (whoAmI != 0xEE && whoAmI != 0xAF) {
        Serial.print("LIDAR not found at 0x29. WHO_AM_I = 0x");
        Serial.println(whoAmI, HEX);
        return false;
    }
    
    // Write new I2C address (2-byte write)
    Wire.beginTransmission(DEFAULT_ADDRESS);
    Wire.write(0x8A);  // I2C_SLAVE_DEVICE_ADDRESS register
    Wire.write(newAddress);
    Wire.endTransmission();
    
    delay(10);
    
    // Verify communication at new address
    Wire.beginTransmission(newAddress);
    Wire.write(REG_IDENTIFICATION_MODEL_ID);
    Wire.endTransmission(false);
    Wire.requestFrom((int)newAddress, 1);
    
    if (!Wire.available()) {
        Serial.print("Failed to communicate at new address 0x");
        Serial.println(newAddress, HEX);
        return false;
    }
    
    Wire.read();
    
    i2cAddress = newAddress;
    
    // Start continuous ranging mode
    writeRegister(REG_SYSRANGE_START, 0x01);
    
    isInitialized = true;
    Serial.print("LIDAR initialized at 0x");
    Serial.println(i2cAddress, HEX);
    
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
    float distance = getDistanceMeters();
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
