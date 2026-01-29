// hardware/pins.h

#ifndef PINS_H
#define PINS_H

// MOTOR DRIVER (L298N) 
#define MOTOR_LEFT_A      26    // in1 -> d26
#define MOTOR_LEFT_B      27    // in2 -> d27
#define MOTOR_LEFT_PWM    25    // ena -> d25

#define MOTOR_RIGHT_A     4     // in3 -> d4
#define MOTOR_RIGHT_B     13    // in4 -> d13
#define MOTOR_RIGHT_PWM   14    // enb -> d14

// ENCODERS 
#define ENCODER_LEFT_A    32    // motor1 c1 -> 32
#define ENCODER_LEFT_B    33    // motor1 c2 -> 33

#define ENCODER_RIGHT_A   34    // motor2 c1 -> 34
#define ENCODER_RIGHT_B   35    // motor2 c2 -> 35

// MPU6050 (I2C)
#define MPU_SDA           21
#define MPU_SCL           22

// LIDAR SENSORS (I2C - VL53L0X/VL53L1X type)
// Shared I2C bus for all LIDARs
#define LIDAR_SDA         21    // Shared with MPU
#define LIDAR_SCL         22    // Shared with MPU

// Front LIDAR
#define LIDAR_FRONT_XSHOT 16    // Address select pin
#define LIDAR_FRONT_INT   17    // Interrupt pin (optional)

// Left LIDAR  
#define LIDAR_LEFT_XSHOT  18    // Address select pin
#define LIDAR_LEFT_INT    19    // Interrupt pin (optional)

// Right LIDAR
#define LIDAR_RIGHT_XSHOT 23    // Address select pin
#define LIDAR_RIGHT_INT   5     // Interrupt pin (optional)

#endif