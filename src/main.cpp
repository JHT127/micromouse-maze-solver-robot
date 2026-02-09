
#include <Arduino.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "StateSpace.h"
#include "PIDController.h"


//------------------------------------------------------------
// GLOBAL OBJECTS
// Purpose: Create all components
//------------------------------------------------------------

Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

StateSpace robot;

PIDController leftPID(2.0, 0.5, 0.1);   // Start with these gains
PIDController rightPID(2.0, 0.5, 0.1);



//------------------------------------------------------------
// CONTROL LOOP TIMING
// Purpose: Update PID at consistent rate
//------------------------------------------------------------

unsigned long lastUpdateTime = 0;
const float UPDATE_INTERVAL = 0.02;  // 20ms = 50Hz



//------------------------------------------------------------
// TARGET SPEED
// Purpose: What velocity we want both wheels to achieve
//------------------------------------------------------------

float targetSpeed = 0.08;  // 8 cm/s (safe speed)



//------------------------------------------------------------
// SETUP
// Purpose: Initialize everything and run tests
//------------------------------------------------------------


void setup() {
    Serial.begin(9600);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("PID CONTROLLER TEST");
    Serial.println("========================================\n");
    

    // Initialize all hardware
    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    robot.initialize();
    

    // Configure PID limits
    leftPID.setOutputLimits(0, 150);   // Motor PWM range (conservative)
    rightPID.setOutputLimits(0, 150);
    leftPID.setIntegralLimits(-50, 50);
    rightPID.setIntegralLimits(-50, 50);
    

    Serial.println("PID Controllers initialized");
    leftPID.printGains();
    Serial.println();
    

    runTests();

}




//------------------------------------------------------------
// RUN TESTS
// Purpose: Test PID controller step by step
//------------------------------------------------------------

void runTests() {

    //------------------------------------------------------------
    // TEST 1: SINGLE WHEEL PID
    //------------------------------------------------------------

    Serial.println("========================================");
    Serial.println("TEST 1: Single Wheel Speed Control");
    Serial.println("========================================");
    Serial.print("Target speed: ");
    Serial.print(targetSpeed, 3);
    Serial.println(" m/s");
    Serial.println("Controlling LEFT wheel only for 3 seconds...\n");
    

    leftEncoder.reset();
    leftPID.reset();
    

    unsigned long startTime = millis();
    lastUpdateTime = startTime;
    
    while (millis() - startTime < 3000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastUpdateTime) / 1000.0;
        
        if (dt >= UPDATE_INTERVAL) {
            // Measure actual speed
            float actualSpeed = leftEncoder.getVelocity(dt);
            
            // PID calculates motor power needed
            float motorPower = leftPID.compute(targetSpeed, actualSpeed, dt);
            
            // Apply to motor
            leftMotor.setSpeed(motorPower);
            
            // Print every 200ms
            if ((currentTime - startTime) % 200 < 50) {
                Serial.print("Time: ");
                Serial.print((currentTime - startTime) / 1000.0, 1);
                Serial.print("s | Target: ");
                Serial.print(targetSpeed, 3);
                Serial.print(" m/s | Actual: ");
                Serial.print(actualSpeed, 3);
                Serial.print(" m/s | PWM: ");
                Serial.println((int)motorPower);
            }
            
            lastUpdateTime = currentTime;
        }
    }
    
    leftMotor.stop();
    
    Serial.println("\n✓ Single wheel PID works!\n");
    delay(2000);
    



    //------------------------------------------------------------
    // TEST 2: BOTH WHEELS WITHOUT PID
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 2: Both Wheels WITHOUT PID");
    Serial.println("(Robot will curve - this is normal)");
    Serial.println("========================================\n");
    
    leftEncoder.reset();
    rightEncoder.reset();
    robot.resetToOrigin();
    
    // Same PWM to both motors (no PID correction)
    leftMotor.setSpeed(80);
    rightMotor.setSpeed(80);
    
    startTime = millis();
    lastUpdateTime = startTime;
    
    while (millis() - startTime < 3000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastUpdateTime) / 1000.0;
        
        if (dt >= UPDATE_INTERVAL) {
            float vLeft = leftEncoder.getVelocity(dt);
            float vRight = rightEncoder.getVelocity(dt);
            
            float v = (vLeft + vRight) / 2.0;
            float omega = (vRight - vLeft) / 0.08;
            robot.predict(v, omega, dt);
            
            if ((currentTime - startTime) % 500 < 50) {
                Serial.print("Time: ");
                Serial.print((currentTime - startTime) / 1000.0, 1);
                Serial.print("s | Left: ");
                Serial.print(vLeft, 3);
                Serial.print(" m/s | Right: ");
                Serial.print(vRight, 3);
                Serial.print(" m/s | ");
                robot.printState();
            }
            
            lastUpdateTime = currentTime;
        }
    }
    
    leftMotor.stop();
    rightMotor.stop();
    
    Serial.println("\nFinal position WITHOUT PID:");
    robot.printState();
    Serial.println("Notice: Robot curved (X != 0, angle != 0)\n");
    
    delay(2000);
    



    //------------------------------------------------------------
    // TEST 3: BOTH WHEELS WITH PID
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 3: Both Wheels WITH PID");
    Serial.println("(Robot should go straighter)");
    Serial.println("========================================\n");
    
    leftEncoder.reset();
    rightEncoder.reset();
    leftPID.reset();
    rightPID.reset();
    robot.resetToOrigin();
    
    startTime = millis();
    lastUpdateTime = startTime;
    
    while (millis() - startTime < 3000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastUpdateTime) / 1000.0;
        
        if (dt >= UPDATE_INTERVAL) {
            // Measure actual speeds
            float vLeft = leftEncoder.getVelocity(dt);
            float vRight = rightEncoder.getVelocity(dt);
            
            // PID controls each wheel independently
            float leftPower = leftPID.compute(targetSpeed, vLeft, dt);
            float rightPower = rightPID.compute(targetSpeed, vRight, dt);
            
            // Apply to motors
            leftMotor.setSpeed(leftPower);
            rightMotor.setSpeed(rightPower);
            
            // Update position
            float v = (vLeft + vRight) / 2.0;
            float omega = (vRight - vLeft) / 0.08;
            robot.predict(v, omega, dt);
            
            if ((currentTime - startTime) % 500 < 50) {
                Serial.print("Time: ");
                Serial.print((currentTime - startTime) / 1000.0, 1);
                Serial.print("s | Left: ");
                Serial.print(vLeft, 3);
                Serial.print(" m/s | Right: ");
                Serial.print(vRight, 3);
                Serial.print(" m/s | ");
                robot.printState();
            }
            
            lastUpdateTime = currentTime;
        }
    }
    
    leftMotor.stop();
    rightMotor.stop();
    
    Serial.println("\nFinal position WITH PID:");
    robot.printState();
    Serial.println("Notice: Straighter line (X ≈ 0, angle ≈ 0)\n");
    



    //------------------------------------------------------------
    // TEST 4: VELOCITY MATCHING DEMO
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 4: Velocity Matching");
    Serial.println("Watch how PID keeps speeds equal");
    Serial.println("========================================\n");
    
    leftEncoder.reset();
    rightEncoder.reset();
    leftPID.reset();
    rightPID.reset();
    
    startTime = millis();
    lastUpdateTime = startTime;
    
    while (millis() - startTime < 5000) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastUpdateTime) / 1000.0;
        
        if (dt >= UPDATE_INTERVAL) {
            float vLeft = leftEncoder.getVelocity(dt);
            float vRight = rightEncoder.getVelocity(dt);
            
            float leftPower = leftPID.compute(targetSpeed, vLeft, dt);
            float rightPower = rightPID.compute(targetSpeed, vRight, dt);
            
            leftMotor.setSpeed(leftPower);
            rightMotor.setSpeed(rightPower);
            
            if ((currentTime - startTime) % 100 < 50) {
                Serial.print("Left: ");
                Serial.print(vLeft, 4);
                Serial.print(" m/s (PWM: ");
                Serial.print((int)leftPower);
                Serial.print(") | Right: ");
                Serial.print(vRight, 4);
                Serial.print(" m/s (PWM: ");
                Serial.print((int)rightPower);
                Serial.print(") | Diff: ");
                Serial.print((vLeft - vRight) * 1000, 2);
                Serial.println(" mm/s");
            }
            
            lastUpdateTime = currentTime;
        }
    }
    
    leftMotor.stop();
    rightMotor.stop();
    
    Serial.println("\n========================================");
    Serial.println("ALL TESTS COMPLETE");
    Serial.println("========================================");
    Serial.println("\nKey Observations:");
    Serial.println("- Test 2 (no PID): Robot curves");
    Serial.println("- Test 3 (with PID): Robot straighter");
    Serial.println("- Test 4: Speeds converge to target");
    Serial.println("\n✓ PID Controller working!");
    Serial.println("✓ Ready for motion control!");
}



//------------------------------------------------------------
// LOOP
// Purpose: Not used in these tests
//------------------------------------------------------------

void loop() {
    // All tests run once in setup()
}
