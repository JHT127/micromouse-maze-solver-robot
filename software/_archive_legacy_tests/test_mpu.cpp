// tests/test_mpu.cpp

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "../hardware/pins.h"

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  // Initialize I2C with your defined pins
  Wire.begin(MPU_SDA, MPU_SCL);

  Serial.println("Initializing MPU6050...");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 connection successful!");

  // Configure sensor ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accel (X,Y,Z): ");
  Serial.print(a.acceleration.x); Serial.print(" ");
  Serial.print(a.acceleration.y); Serial.print(" ");
  Serial.print(a.acceleration.z);

  Serial.print(" | Gyro (X,Y,Z): ");
  Serial.print(g.gyro.x); Serial.print(" ");
  Serial.print(g.gyro.y); Serial.print(" ");
  Serial.print(g.gyro.z);

  Serial.print(" | Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  delay(200);
}
