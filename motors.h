#ifndef MOTORS_H
#define MOTORS_H

#include <avr/io.h>

/* Direction constants */
#define FORWARD   1
#define BACKWARD -1

/* Motor identifiers */
#define MOTOR_LEFT   0
#define MOTOR_RIGHT  1

/* This value scales your floating “speed” into 0–255 or 0–MOTOR_TOP_SPEED range.
   If you’re using an 8-bit timer with Fast PWM (0–255 range), adjust accordingly. */
#define MOTOR_TOP_SPEED 200

/* Motor control pins (adjust as needed for your hardware) */
#define LEFT_CONTROL PD2   // direction for left motor
#define LEFT_PWM     PD5   // PWM pin for left motor
#define RIGHT_CONTROL PD4  // direction for right motor
#define RIGHT_PWM     PD6  // PWM pin for right motor

/* Disable both motors because you usually disable or enable them together. */
void disableMotors(void);
void enableMotors(void);

/* Set up the PWM on the motors and initialize them to off */
void setupMotors(void);

/* Set the direction and speed of one of the motors */
void setMotorSpeed(int motor, double speed, int direction);

/* Follow the wall by adjusting motor speeds (requires ultrasonic logic) */
void followWall(double percentPower, double maintainDistance, int side);

#endif
