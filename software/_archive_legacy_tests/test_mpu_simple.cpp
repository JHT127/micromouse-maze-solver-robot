#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "../hardware/pins.h"

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  Wire.begin(MPU_SDA, MPU_SCL);

  Serial.println("Initializing MPU6050...");

  // Try 0x68 first, then 0x69
  if (!mpu.begin(0x68)) {
    Serial.println("No response at 0x68, trying 0x69...");
    if (!mpu.begin(0x69)) {
      Serial.println("Failed to find MPU6050 at either address!");
      while (1) delay(10);
    }
  }
  Serial.println("MPU6050 connection successful!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accel: ");
  Serial.print(a.acceleration.x); Serial.print(", ");
  Serial.print(a.acceleration.y); Serial.print(", ");
  Serial.print(a.acceleration.z);

  Serial.print(" | Gyro: ");
  Serial.print(g.gyro.x); Serial.print(", ");
  Serial.print(g.gyro.y); Serial.print(", ");
  Serial.print(g.gyro.z);

  Serial.print(" | Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  delay(200);
}
