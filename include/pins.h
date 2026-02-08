#ifndef PINS_H
#define PINS_H


//------------------------------------------------------------
// MOTOR DRIVER PINS (L298N)
// Purpose: Define which ESP32 pins connect to motor driver
//------------------------------------------------------------
#define MOTOR_LEFT_A      26
#define MOTOR_LEFT_B      27
#define MOTOR_LEFT_PWM    25

#define MOTOR_RIGHT_A     4
#define MOTOR_RIGHT_B     13
#define MOTOR_RIGHT_PWM   14



//------------------------------------------------------------
// ENCODER PINS
// Purpose: Define which ESP32 pins connect to encoders
//------------------------------------------------------------
#define ENCODER_LEFT_A    32
#define ENCODER_LEFT_B    33

#define ENCODER_RIGHT_A   34
#define ENCODER_RIGHT_B   35



//------------------------------------------------------------
// MPU6050 PINS (I2C)
// Purpose: Gyroscope sensor pins
//------------------------------------------------------------
#define MPU_SDA           21
#define MPU_SCL           22



//------------------------------------------------------------
// LIDAR SENSOR PINS (I2C)
// Purpose: Distance sensors for wall detection
//------------------------------------------------------------
#define LIDAR_SDA         21
#define LIDAR_SCL         22

#define LIDAR_FRONT_XSHOT 16
#define LIDAR_FRONT_INT   17

#define LIDAR_LEFT_XSHOT  18
#define LIDAR_LEFT_INT    19

#define LIDAR_RIGHT_XSHOT 23
#define LIDAR_RIGHT_INT   5

#endif