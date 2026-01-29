// tests/test_i2c_scan.cpp

#include <Arduino.h>
#include <Wire.h>
#include "../hardware/pins.h"

void setup() {
  Serial.begin(9600);
  // Use your defined pins for I2C
  Wire.begin(MPU_SDA, MPU_SCL);

  Serial.println("I2C Scanner - starting...");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);

      // Highlight MPU6050 addresses
      if (address == 0x68 || address == 0x69) {
        Serial.print("  <-- Possible MPU6050");
      }

      Serial.println();
      nDevices++;
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("Scan complete");
  }

  delay(2000); // wait before next scan
}
