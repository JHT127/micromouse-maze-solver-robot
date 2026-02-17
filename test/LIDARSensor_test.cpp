
#include <Arduino.h>
#include "pins.h"
#include "LIDARSensor.h"

//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: Three LIDAR sensors
//------------------------------------------------------------

LIDARSensor frontLidar(LIDAR_FRONT_XSHOT, LIDAR_FRONT_INT);
LIDARSensor leftLidar(LIDAR_LEFT_XSHOT, LIDAR_LEFT_INT);
LIDARSensor rightLidar(LIDAR_RIGHT_XSHOT, LIDAR_RIGHT_INT);

//------------------------------------------------------------
// SETUP
// Purpose: Initialize LIDARs with sequential addressing
//------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("LIDAR SENSOR TEST");
    Serial.println("========================================\n");
    
    // Test 1: Disable all sensors first
    Serial.println("========================================");
    Serial.println("TEST 1: Sequential Initialization");
    Serial.println("========================================");
    Serial.println("Disabling all sensors...");
    
    frontLidar.disable();
    leftLidar.disable();
    rightLidar.disable();
    delay(50);
    
    Serial.println("✓ All disabled\n");
    
    // Test 2: Initialize front LIDAR
    Serial.println("Initializing FRONT LIDAR at 0x30...");
    frontLidar.enable();
    delay(50);
    
    if (!frontLidar.initialize(0x30)) {
        Serial.println("✗ FAILED: Front LIDAR not found");
        Serial.println("  Check I2C wiring (pins 21, 22)");
        Serial.println("  Check XSHUT pin 16");
        Serial.println("  Verify pullup resistors");
        while(1) delay(1000);
    }
    Serial.println("✓ Front LIDAR ready\n");
    
    // Test 3: Initialize left LIDAR
    Serial.println("Initializing LEFT LIDAR at 0x31...");
    leftLidar.enable();
    delay(50);
    
    if (!leftLidar.initialize(0x31)) {
        Serial.println("✗ FAILED: Left LIDAR not found");
        while(1) delay(1000);
    }
    Serial.println("✓ Left LIDAR ready\n");
    
    // Test 4: Initialize right LIDAR
    Serial.println("Initializing RIGHT LIDAR at 0x32...");
    rightLidar.enable();
    delay(50);
    
    if (!rightLidar.initialize(0x32)) {
        Serial.println("✗ FAILED: Right LIDAR not found");
        while(1) delay(1000);
    }
    Serial.println("✓ Right LIDAR ready\n");
    
    Serial.println("========================================");
    Serial.println("TEST 2: Distance Readings");
    Serial.println("========================================");
    Serial.println("All three LIDAR sensors ready!");
    Serial.println("Place hand in front of each sensor\n");
}

//------------------------------------------------------------
// LOOP
// Purpose: Read and display distance from all sensors
//------------------------------------------------------------

void loop() {
    uint16_t frontDist = frontLidar.getDistanceMM();
    uint16_t leftDist = leftLidar.getDistanceMM();
    uint16_t rightDist = rightLidar.getDistanceMM();
    
    Serial.print("Front: ");
    Serial.print(frontDist);
    Serial.print("mm");
    if (frontLidar.isWallDetected()) Serial.print(" [WALL]");
    
    Serial.print("  |  Left: ");
    Serial.print(leftDist);
    Serial.print("mm");
    if (leftLidar.isWallDetected()) Serial.print(" [WALL]");
    
    Serial.print("  |  Right: ");
    Serial.print(rightDist);
    Serial.print("mm");
    if (rightLidar.isWallDetected()) Serial.print(" [WALL]");
    
    Serial.println();
    
    delay(200);
}
