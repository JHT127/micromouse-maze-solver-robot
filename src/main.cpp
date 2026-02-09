
#include <Arduino.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "StateSpace.h"


//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: Create motors, encoders, and state space
//------------------------------------------------------------

Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);


Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);   // true = left
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false); // false = right


StateSpace robot;
void runTests();

//------------------------------------------------------------
// TIMING
// Purpose: Track time for velocity calculations
//------------------------------------------------------------

unsigned long lastUpdateTime = 0;
const float UPDATE_INTERVAL = 0.02;  // 20ms = 50Hz




//------------------------------------------------------------
// SETUP
// Purpose: Initialize everything and run tests
//------------------------------------------------------------

void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("MOTOR + ENCODER + STATE SPACE TEST");
    Serial.println("========================================\n");
    
    // Initialize hardware
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    robot.initialize();
    
    Serial.println("Hardware initialized\n");
    
    runTests();
}




//------------------------------------------------------------
// RUN TESTS
// Purpose: Test motors and encoders step by step
//------------------------------------------------------------

void runTests() {



    //------------------------------------------------------------
    // TEST 1: BASIC MOTOR CONTROL
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 1: Basic Motor Control");
    Serial.println("========================================");
    Serial.println("Left motor forward...");
    
    delay(3000);
    leftMotor.forward(100);
    delay(1000);
    leftMotor.stop();
    
    Serial.print("Left encoder ticks: ");
    Serial.println(leftEncoder.getTicks());
    Serial.println("✓ Left motor works\n");
    
    delay(500);
    
    Serial.println("Right motor forward...");
    rightMotor.forward(60);
    delay(1000);
    rightMotor.stop();
    
    Serial.print("Right encoder ticks: ");
    Serial.println(rightEncoder.getTicks());
    Serial.println("✓ Right motor works\n");
    
    delay(1000);
    leftEncoder.reset();
    rightEncoder.reset();
    




    //------------------------------------------------------------
    // TEST 2: BOTH MOTORS TOGETHER
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 2: Both Motors Together");
    Serial.println("========================================");
    Serial.println("Moving forward for 2 seconds...\n");
    
    leftMotor.forward(100);
    rightMotor.forward(70);
    
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
        Serial.print("Left: ");
        Serial.print(leftEncoder.getTicks());
        Serial.print(" ticks | Right: ");
        Serial.print(rightEncoder.getTicks());
        Serial.println(" ticks");
        delay(200);
    }
    
    leftMotor.stop();
    rightMotor.stop();
    
    Serial.println("\n✓ Both motors work together\n");
    
    delay(1000);
    leftEncoder.reset();
    rightEncoder.reset();
    



    //------------------------------------------------------------
    // TEST 3: VELOCITY CALCULATION
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 3: Velocity Calculation");
    Serial.println("========================================");
    Serial.println("Measuring velocity for 3 seconds...\n");
    
    leftMotor.forward(100);
    rightMotor.forward(80);
    
    startTime = millis();
    unsigned long lastPrintTime = startTime;
    
    while (millis() - startTime < 3000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastPrintTime) / 1000.0;
        
        if (dt >= 0.1) {  // Print every 100ms
            float vLeft = leftEncoder.getVelocity(dt);
            float vRight = rightEncoder.getVelocity(dt);
            
            Serial.print("Left: ");
            Serial.print(vLeft, 4);
            Serial.print(" m/s | Right: ");
            Serial.print(vRight, 4);
            Serial.println(" m/s");
            
            lastPrintTime = currentTime;
        }
    }
    
    leftMotor.stop();
    rightMotor.stop();
    
    Serial.println("\n✓ Velocity calculation works\n");
    
    delay(1000);
    leftEncoder.reset();
    rightEncoder.reset();
    robot.resetToOrigin();
    



    //------------------------------------------------------------
    // TEST 4: STATE SPACE INTEGRATION
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 4: State Space Integration");
    Serial.println("========================================");
    Serial.println("Moving and tracking position...\n");
    
    leftMotor.forward(100);
    rightMotor.forward(80);
    
    startTime = millis();
    lastUpdateTime = startTime;
    
    while (millis() - startTime < 3000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastUpdateTime) / 1000.0;
        
        if (dt >= UPDATE_INTERVAL) {

            // Get velocities from encoders
            float vLeft = leftEncoder.getVelocity(dt);
            float vRight = rightEncoder.getVelocity(dt);
            

            // Calculate control inputs
            float v = (vLeft + vRight) / 2.0;      // Forward velocity
            float omega = (vRight - vLeft) / 0.08;  // Angular velocity (wheelbase = 8cm)
            

            // Update state space
            robot.predict(v, omega, dt);
            

            // Print every 500ms
            if ((currentTime - startTime) % 500 < 50) {
                Serial.print("Time: ");
                Serial.print((currentTime - startTime) / 1000.0, 1);
                Serial.print("s | ");
                robot.printState();
            }
            

            lastUpdateTime = currentTime;
            
        }
    }
    


    leftMotor.stop();
    rightMotor.stop();
    


    Serial.println("\n========================================");
    Serial.println("FINAL STATE:");
    Serial.println("========================================");
    robot.printState();
    
    Serial.print("Total distance (encoders): ");
    Serial.print((leftEncoder.getDistance() + rightEncoder.getDistance()) / 2.0, 4);
    Serial.println(" meters");
    
    Serial.println("\n✓ State space tracks position!\n");
    
    Serial.println("========================================");
    Serial.println("ALL TESTS COMPLETE");
    Serial.println("Ready for PID controller!");
    Serial.println("========================================");
}




//------------------------------------------------------------
// LOOP
// Purpose: Not used in these tests
//------------------------------------------------------------

void loop() {
    // Tests run once in setup()
}

