#include "MPU6050Driver.h"

//------------------------------------------------------------
// CONSTRUCTOR
// Purpose: Initialize sensor state
//------------------------------------------------------------
MPU6050Driver::MPU6050Driver() {
    gyroBiasZ = 0.0;
    isCalibrated = false;
    currentAngle = 0.0;
    lastUpdateTime = millis();
}

//------------------------------------------------------------
// INITIALIZE
// Purpose: Configure MPU6050 on I2C bus
//------------------------------------------------------------
bool MPU6050Driver::initialize() {
    Wire.begin();
    delay(100);
    
    // Verify sensor is present
    uint8_t whoAmI = readRegister(MPU6050_WHO_AM_I);
    if (whoAmI != 0x68) {
        Serial.print("MPU6050 not found. WHO_AM_I = 0x");
        Serial.println(whoAmI, HEX);
        return false;
    }
    
    // Wake up MPU6050 (clear sleep bit)
    writeRegister(MPU6050_PWR_MGMT_1, 0x00);
    delay(100);
    
    // Set gyro range to ±250°/s
    writeRegister(MPU6050_GYRO_CONFIG, 0x00);
    delay(100);
    
    Serial.println("MPU6050 initialized successfully");
    return true;
}

//------------------------------------------------------------
// CALIBRATE
// Purpose: Measure gyro bias during stationary period
//------------------------------------------------------------
bool MPU6050Driver::calibrate(int samples) {
    Serial.print("Calibrating MPU6050 with ");
    Serial.print(samples);
    Serial.println(" samples...");
    
    float sum = 0.0;
    
    for (int i = 0; i < samples; i++) {
        int16_t raw = readGyroZ();
        float angularVel = (raw / GYRO_SCALE) * DEG_TO_RAD;
        sum += angularVel;
        delay(10);
    }
    
    gyroBiasZ = sum / samples;
    isCalibrated = true;
    
    Serial.print("Gyro bias Z: ");
    Serial.println(gyroBiasZ, 6);
    
    return true;
}

//------------------------------------------------------------
// UPDATE
// Purpose: Integrate gyro reading to get angle
//------------------------------------------------------------
void MPU6050Driver::update(float dt) {
    unsigned long now = millis();
    float actualDt = (now - lastUpdateTime) / 1000.0;
    lastUpdateTime = now;
    
    int16_t raw = readGyroZ();
    float angularVel = (raw / GYRO_SCALE) * DEG_TO_RAD - gyroBiasZ;
    
    // Integrate
    currentAngle += angularVel * actualDt;
    
    // Normalize to [-π, π]
    while (currentAngle > PI) currentAngle -= 2 * PI;
    while (currentAngle < -PI) currentAngle += 2 * PI;
}

//------------------------------------------------------------
// GET ANGULAR VELOCITY
// Purpose: Return current rotation speed in rad/s
//------------------------------------------------------------
float MPU6050Driver::getAngularVelocity() {
    int16_t raw = readGyroZ();
    float angularVel = (raw / GYRO_SCALE) * DEG_TO_RAD - gyroBiasZ;
    return angularVel;
}

//------------------------------------------------------------
// GET ANGLE
// Purpose: Return integrated angle in radians
//------------------------------------------------------------
float MPU6050Driver::getAngle() {
    return currentAngle;
}

//------------------------------------------------------------
// GET ANGLE DEGREES
// Purpose: Return integrated angle in degrees
//------------------------------------------------------------
float MPU6050Driver::getAngleDegrees() {
    return currentAngle * 180.0 / PI;
}

//------------------------------------------------------------
// RESET ANGLE
// Purpose: Zero out integrated angle
//------------------------------------------------------------
void MPU6050Driver::resetAngle() {
    currentAngle = 0.0;
    lastUpdateTime = millis();
}

//------------------------------------------------------------
// SET ANGLE
// Purpose: Manually set angle in radians
//------------------------------------------------------------
void MPU6050Driver::setAngle(float angle) {
    currentAngle = angle;
}

//------------------------------------------------------------
// IS READY
// Purpose: Check if calibrated
//------------------------------------------------------------
bool MPU6050Driver::isReady() {
    return isCalibrated;
}

//------------------------------------------------------------
// WRITE REGISTER
// Purpose: I2C write to register
//------------------------------------------------------------
void MPU6050Driver::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

//------------------------------------------------------------
// READ REGISTER
// Purpose: I2C read from register
//------------------------------------------------------------
uint8_t MPU6050Driver::readRegister(uint8_t reg) {
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((int)MPU6050_ADDRESS, 1);
    
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

//------------------------------------------------------------
// READ GYRO Z
// Purpose: Read 16-bit Z-axis gyro value
//------------------------------------------------------------
int16_t MPU6050Driver::readGyroZ() {
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_GYRO_ZOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom((int)MPU6050_ADDRESS, 2);
    
    int16_t value = 0;
    if (Wire.available()) {
        uint8_t msb = Wire.read();
        uint8_t lsb = Wire.read();
        value = (msb << 8) | lsb;
    }
    return value;
}
