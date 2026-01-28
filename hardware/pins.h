// hardware/pins.h

#ifndef PINS_H
#define PINS_H

// MOTOR DRIVER (L298N) 
#define MOTOR_LEFT_A      26    // in1 -> d26
#define MOTOR_LEFT_B      27    // in2 -> d27
#define MOTOR_LEFT_PWM    25    // ena -> d25

#define MOTOR_RIGHT_A     12    // in3 -> d12
#define MOTOR_RIGHT_B     13    // in4 -> d13
#define MOTOR_RIGHT_PWM   14    // enb -> d14

// ENCODERS 
#define ENCODER_LEFT_A    32    // motor1 c1 -> 32
#define ENCODER_LEFT_B    33    // motor1 c2 -> 33

#define ENCODER_RIGHT_A   34    // motor2 c1 -> 34
#define ENCODER_RIGHT_B   35    // motor2 c2 -> 35

#endif