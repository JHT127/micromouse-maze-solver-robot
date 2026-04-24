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

// Tuned PID coefficients for velocity control
// Kp: proportional gain - high enough to overcome motor friction
// Ki: integral gain - reduces steady-state error
// Kd: derivative gain - dampens oscillations
PIDController leftPID(4100.0, 0.0, 0.0);
PIDController rightPID(4100.0, 0.0, 0.0);

//------------------------------------------------------------
// TIMING
//------------------------------------------------------------
unsigned long lastUpdateTime = 0;
const float UPDATE_INTERVAL = 0.02;  // 20ms update rate
float targetSpeed = 0.08;  // Target velocity in m/s

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

    leftPID.setOutputLimits(0, 255);
    rightPID.setOutputLimits(0, 255);
    leftPID.setIntegralLimits(-120, 120);
    rightPID.setIntegralLimits(-120, 120);

    Serial.println("All hardware initialized");
    leftPID.printGains();
    Serial.println();

    //------------------------------------------------------------
    // TEST 1: SINGLE WHEEL PID
    //------------------------------------------------------------
    Serial.println("========================================");
    Serial.println("TEST 1: Single Wheel PID (Left only)");
    Serial.println("========================================");
    Serial.print("Target: ");
    Serial.print(targetSpeed);
    Serial.println(" m/s");
    //Serial.println("Kp=500.0, Ki=28.0, Kd=6.0");
    Serial.println("Output range: 0-255 PWM (smooth stable control)\n");

    leftEncoder.reset();
    leftPID.reset();
    lastUpdateTime = millis();

    unsigned long startTime = millis();
    while (millis() - startTime < 4000) {
        unsigned long now = millis();
        float dt = (now - lastUpdateTime) / 1000.0;

        if (dt >= UPDATE_INTERVAL) {
            float actual = leftEncoder.getVelocity(dt);
            float power = leftPID.compute(targetSpeed, actual, dt);
            leftMotor.setSpeed(power);

            if ((now - startTime) % 500 < 30) {
                float error = targetSpeed - actual;
                Serial.print("t=");
                Serial.print((now - startTime) / 1000.0, 1);
                Serial.print("s | target=");
                Serial.print(targetSpeed, 3);
                Serial.print(" | actual=");
                Serial.print(actual, 3);
                Serial.print(" | error=");
                Serial.print(error, 3);
                Serial.print(" | PWM=");
                Serial.print((int)power);
                Serial.print(" | integral=");
                Serial.println(leftPID.getIntegral(), 2);
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
    Serial.println("========================================");
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
    
    Serial.println("\n⏳ Repositioning robot to start position...");
    delay(5000);  // 5 second delay for user to move robot

    //------------------------------------------------------------
    // TEST 3: BOTH WHEELS WITH PID
    //------------------------------------------------------------
    Serial.println("\n========================================");
    Serial.println("TEST 3: Both Wheels WITH PID");
    Serial.println("========================================");
    Serial.print("Target: ");
    Serial.print(targetSpeed);
    Serial.println(" m/s");
    //Serial.println("Kp=500.0, Ki=28.0, Kd=6.0\n");

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
                Serial.print("t=");
                Serial.print((now - startTime) / 1000.0, 1);
                Serial.print("s | ");
                Serial.print("L=");
                Serial.print(vL, 3);
                Serial.print(" m/s");
                Serial.print(" | R=");
                Serial.print(vR, 3);
                Serial.print(" m/s | ");
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
    Serial.println("✓ ALL TESTS COMPLETE");
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
