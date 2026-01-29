// tests/test_mpu.cpp

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "../hardware/pins.h"

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C with the defined pins
  Wire.begin(MPU_SDA, MPU_SCL);
  
  Serial.println("Initializing MPU6050...");

  // Try to initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Configure MPU6050 settings
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G: Serial.println("±2 G"); break;
    case MPU6050_RANGE_4_G: Serial.println("±4 G"); break;
    case MPU6050_RANGE_8_G: Serial.println("±8 G"); break;
    case MPU6050_RANGE_16_G: Serial.println("±16 G"); break;
  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG: Serial.println("±250 deg/s"); break;
    case MPU6050_RANGE_500_DEG: Serial.println("±500 deg/s"); break;
    case MPU6050_RANGE_1000_DEG: Serial.println("±1000 deg/s"); break;
    case MPU6050_RANGE_2000_DEG: Serial.println("±2000 deg/s"); break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ: Serial.println("260 Hz"); break;
    case MPU6050_BAND_184_HZ: Serial.println("184 Hz"); break;
    case MPU6050_BAND_94_HZ: Serial.println("94 Hz"); break;
    case MPU6050_BAND_44_HZ: Serial.println("44 Hz"); break;
    case MPU6050_BAND_21_HZ: Serial.println("21 Hz"); break;
    case MPU6050_BAND_10_HZ: Serial.println("10 Hz"); break;
    case MPU6050_BAND_5_HZ: Serial.println("5 Hz"); break;
  }

  Serial.println("MPU6050 initialized successfully!");
  delay(100);
}

void loop() {
  // Get new sensor events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Print acceleration data
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s², ");
  Serial.print("Y: "); Serial.print(a.acceleration.y); Serial.print(" m/s², ");
  Serial.print("Z: "); Serial.print(a.acceleration.z); Serial.println(" m/s²");

  // Print gyro data
  Serial.print("Gyro X: "); Serial.print(g.gyro.x); Serial.print(" rad/s, ");
  Serial.print("Y: "); Serial.print(g.gyro.y); Serial.print(" rad/s, ");
  Serial.print("Z: "); Serial.print(g.gyro.z); Serial.println(" rad/s");

  // Print temperature
  Serial.print("Temp: "); Serial.print(temp.temperature); Serial.println(" °C");

  Serial.println("-----------------------------------");
  delay(500);
}