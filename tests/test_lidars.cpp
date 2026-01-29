// tests/test_lidars.cpp

#include <Wire.h>
#include <VL53L0X.h>
#include "../hardware/pins.h"

VL53L0X frontLidar;
VL53L0X leftLidar;
VL53L0X rightLidar;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Set XSHUT pins as outputs
  pinMode(LIDAR_FRONT_XSHOT, OUTPUT);
  pinMode(LIDAR_LEFT_XSHOT, OUTPUT);
  pinMode(LIDAR_RIGHT_XSHOT, OUTPUT);

  // 1️. Turn OFF all LiDARs (XSHUT active LOW for shutdown)
  digitalWrite(LIDAR_FRONT_XSHOT, LOW);
  digitalWrite(LIDAR_LEFT_XSHOT, LOW);
  digitalWrite(LIDAR_RIGHT_XSHOT, LOW);
  delay(50);

  // 2️. Turn ON front LiDAR and change address
  digitalWrite(LIDAR_FRONT_XSHOT, HIGH);
  delay(10);
  if (!frontLidar.init()) {
    Serial.println("Failed to initialize front LiDAR!");
  }
  frontLidar.setAddress(0x30);

  // 3️. Turn ON left LiDAR and change address
  digitalWrite(LIDAR_LEFT_XSHOT, HIGH);
  delay(10);
  if (!leftLidar.init()) {
    Serial.println("Failed to initialize left LiDAR!");
  }
  leftLidar.setAddress(0x31);

  // 4️. Turn ON right LiDAR and change address
  digitalWrite(LIDAR_RIGHT_XSHOT, HIGH);
  delay(10);
  if (!rightLidar.init()) {
    Serial.println("Failed to initialize right LiDAR!");
  }
  rightLidar.setAddress(0x32);

  // Optional: better accuracy
  frontLidar.setMeasurementTimingBudget(20000);
  leftLidar.setMeasurementTimingBudget(20000);
  rightLidar.setMeasurementTimingBudget(20000);

  // Start continuous measurements
  frontLidar.startContinuous();
  leftLidar.startContinuous();
  rightLidar.startContinuous();

  Serial.println("All LiDARs initialized!");
}

void loop() {
  uint16_t front = frontLidar.readRangeContinuousMillimeters();
  uint16_t left  = leftLidar.readRangeContinuousMillimeters();
  uint16_t right = rightLidar.readRangeContinuousMillimeters();

  Serial.print("Front: "); Serial.print(front);
  Serial.print(" mm | Left: "); Serial.print(left);
  Serial.print(" mm | Right: "); Serial.print(right);
  Serial.println(" mm");

  delay(100);
}