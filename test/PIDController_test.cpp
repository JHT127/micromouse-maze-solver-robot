#include <Arduino.h>
#include "pins.h"
#include "Motor.h"
#include "Encoder.h"
#include "StateSpace.h"
#include "PIDController.h"

//------------------------------------------------------------
// GLOBAL OBJECTS
//------------------------------------------------------------
Motor leftMotor(MOTOR_LEFT_A, MOTOR_LEFT_B, MOTOR_LEFT_PWM);
Motor rightMotor(MOTOR_RIGHT_A, MOTOR_RIGHT_B, MOTOR_RIGHT_PWM);

Encoder leftEncoder(ENCODER_LEFT_A, ENCODER_LEFT_B, true);
Encoder rightEncoder(ENCODER_RIGHT_A, ENCODER_RIGHT_B, false);

StateSpace robot;

PIDController leftPID(500.0, 20.0, 10.0);
PIDController rightPID(500.0, 20.0, 10.0);

//------------------------------------------------------------
// TIMING
//------------------------------------------------------------
unsigned long lastUpdateTime = 0;
const float UPDATE_INTERVAL = 0.02;
float targetSpeed = 0.15;

bool testsDone = false;

void setup() {
    Serial.begin(9600);
    delay(3000);  // Extra time to open serial monitor

    Serial.println("========================================");
    Serial.println("PID CONTROLLER TEST");
    Serial.println("========================================\n");

    leftMotor.initialize();
    rightMotor.initialize();
    leftEncoder.initialize();
    rightEncoder.initialize();
    robot.initialize();

    leftPID.setOutputLimits(0,200);
    rightPID.setOutputLimits(0,200);
    leftPID.setIntegralLimits(-50, 50);
    rightPID.setIntegralLimits(-50, 50);

    Serial.println("All hardware initialized");
    leftPID.printGains();
    Serial.println();

    //------------------------------------------------------------
    // SANITY CHECK: Verify motors work before PID
    //------------------------------------------------------------
    /*Serial.println("========================================");
    Serial.println("SANITY CHECK: Motors and Encoders");
    Serial.println("========================================");
    Serial.println("Spinning both motors at PWM 80 for 2 seconds...\n");

    leftEncoder.reset();
    rightEncoder.reset();

    leftMotor.setSpeed(80);
    rightMotor.setSpeed(80);

    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
        Serial.print("Left ticks: ");
        Serial.print(leftEncoder.getTicks());
        Serial.print(" | Right ticks: ");
        Serial.println(rightEncoder.getTicks());
        delay(400);
    }

    leftMotor.stop();
    rightMotor.stop();

    Serial.print("\nFinal - Left: ");
    Serial.print(leftEncoder.getTicks());
    Serial.print(" | Right: ");
    Serial.println(rightEncoder.getTicks());

    if (leftEncoder.getTicks() < 10 || rightEncoder.getTicks() < 10) {
        Serial.println("\n❌ ERROR: Encoders not reading!");
        Serial.println("Fix encoders before PID test.");
        Serial.println("Stopping here.");
        while(true) { delay(1000); }  // Stop everything
    }

    Serial.println("\n✓ Motors and encoders working!\n");
    delay(2000);*/

    //------------------------------------------------------------
    // TEST 1: SINGLE WHEEL PID
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 1: Single Wheel PID (Left only)");
    Serial.println("========================================");
    Serial.print("Target: ");
    Serial.print(targetSpeed);
    Serial.println(" m/s\n");

    leftEncoder.reset();
    leftPID.reset();
    lastUpdateTime = millis();

    //remove
    unsigned long startTime = millis();

    startTime = millis();
    while (millis() - startTime < 4000) {
        unsigned long now = millis();
        float dt = (now - lastUpdateTime) / 1000.0;

        if (dt >= UPDATE_INTERVAL) {
            float actual = leftEncoder.getVelocity(dt);
            float power = leftPID.compute(targetSpeed, actual, dt);
            leftMotor.setSpeed(power);

            if ((now - startTime) % 500 < 30) {
                Serial.print("t=");
                Serial.print((now - startTime) / 1000.0, 1);
                Serial.print("s | actual=");
                Serial.print(actual, 3);
                Serial.print(" m/s | PWM=");
                Serial.println((int)power);
            }

            lastUpdateTime = now;
        }
    }

    leftMotor.stop();
    Serial.println("\n✓ Test 1 done\n");
    delay(2000);

    //------------------------------------------------------------
    // TEST 2: BOTH WHEELS WITHOUT PID (Baseline)
    //------------------------------------------------------------
    /*Serial.println("========================================");
    Serial.println("TEST 2: Both Wheels WITHOUT PID");
    Serial.println("========================================\n");

    leftEncoder.reset();
    rightEncoder.reset();
    robot.resetToOrigin();

    leftMotor.setSpeed(80);
    rightMotor.setSpeed(80);

    startTime = millis();
    lastUpdateTime = startTime;

    while (millis() - startTime < 3000) {
        unsigned long now = millis();
        float dt = (now - lastUpdateTime) / 1000.0;

        if (dt >= UPDATE_INTERVAL) {
            float vL = leftEncoder.getVelocity(dt);
            float vR = rightEncoder.getVelocity(dt);
            float v = (vL + vR) / 2.0;
            float omega = (vR - vL) / 0.08;
            robot.predict(v, omega, dt);

            if ((now - startTime) % 500 < 30) {
                Serial.print("L=");
                Serial.print(vL, 3);
                Serial.print(" R=");
                Serial.print(vR, 3);
                Serial.print(" | ");
                robot.printState();
            }

            lastUpdateTime = now;
        }
    }

    leftMotor.stop();
    rightMotor.stop();

    Serial.println("\nFinal WITHOUT PID:");
    robot.printState();
    delay(2000);*/

    //------------------------------------------------------------
    // TEST 3: BOTH WHEELS WITH PID
    //------------------------------------------------------------
    Serial.println("\n========================================");
    Serial.println("TEST 3: Both Wheels WITH PID");
    Serial.println("========================================\n");

    leftEncoder.reset();
    rightEncoder.reset();
    leftPID.reset();
    rightPID.reset();
    robot.resetToOrigin();

    startTime = millis();
    lastUpdateTime = startTime;

    while (millis() - startTime < 3000) {
        unsigned long now = millis();
        float dt = (now - lastUpdateTime) / 1000.0;

        if (dt >= UPDATE_INTERVAL) {
            float vL = leftEncoder.getVelocity(dt);
            float vR = rightEncoder.getVelocity(dt);

            float leftPower = leftPID.compute(targetSpeed, vL, dt);
            float rightPower = rightPID.compute(targetSpeed, vR, dt);

            leftMotor.setSpeed(leftPower);
            rightMotor.setSpeed(rightPower);

            float v = (vL + vR) / 2.0;
            float omega = (vR - vL) / 0.08;
            robot.predict(v, omega, dt);

            if ((now - startTime) % 500 < 30) {
                Serial.print("L=");
                Serial.print(vL, 3);
                Serial.print(" R=");
                Serial.print(vR, 3);
                Serial.print(" | ");
                robot.printState();
            }

            lastUpdateTime = now;
        }
    }

    leftMotor.stop();
    rightMotor.stop();

    Serial.println("\nFinal WITH PID:");
    robot.printState();

    Serial.println("\n========================================");
    Serial.println("ALL TESTS COMPLETE");
    Serial.println("========================================");

    testsDone = true;
}

//------------------------------------------------------------
// LOOP
// Purpose: Do nothing after tests finish
//------------------------------------------------------------
void loop() {
    delay(10000);  // Sit here forever, no re-running
}
