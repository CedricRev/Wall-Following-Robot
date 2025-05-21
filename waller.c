#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "motors.h"
#include "ultrasonic.h"
#include "timer0.h"

static const double originalSpeed = 0.30;
static const double distanceThreshold = 8.0;
static double percentSpeed = 0.30;
static double percentSpeed_left = 0.30;

static bool firstWall = false;
static bool secondWall = false;

static uint32_t clock = 0;


static void too_far(void) {
    // forward
    // setMotorSpeed(MOTOR_RIGHT, percentSpeed, FORWARD);
    // setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    // _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(500);

    // turn Right
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, BACKWARD);
    _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
    _delay_ms(500);

    // forward
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, FORWARD);
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    _delay_ms(800);

    // stop
    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(500);

}


static void tilt_right(void) {

    // turn Right
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, BACKWARD);
    _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
    _delay_ms(500);

    // forward
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, FORWARD);
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    _delay_ms(800);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
    _delay_ms(500);

    // pivot left
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, BACKWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, FORWARD);
    _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(500);
}

static void too_close(void)
{
    // Move backward
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, BACKWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, BACKWARD);
    _delay_ms(450);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
    _delay_ms(50);

    // pivot right
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, BACKWARD);
    _delay_ms(500);

    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(200);

    // Move backward again
    setMotorSpeed(MOTOR_LEFT,  percentSpeed, BACKWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed, BACKWARD);
    _delay_ms(450);

    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(200);

    // Another pivot left
    setMotorSpeed(MOTOR_LEFT,  percentSpeed + 0.3, BACKWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed + 0.3, FORWARD);
    _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);

    setMotorSpeed(MOTOR_RIGHT, 0, BACKWARD);
    setMotorSpeed(MOTOR_LEFT,  0, BACKWARD);
    _delay_ms(200);
}

static void tilt_left(void) {
    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
    _delay_ms(50);

    // rotate left
    setMotorSpeed(MOTOR_LEFT,  percentSpeed_left + 0.3, BACKWARD);
    setMotorSpeed(MOTOR_RIGHT, percentSpeed_left + 0.3, FORWARD);
    _delay_ms(300);

    // stop
    setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
    setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);
}

int main(void) {
    sei(); // Global Interrupt

    setupTimer0(); // Set 1ms per tick
    setupMotors();  
    setupUltrasonic(); // Ultrasonic for distance
    setupServo(); // Servo for tilting ultrasonic
    setupLed(); // Led for LAB 5

    clock = getTime(); // Get new timer clock to set as starting point

    while (1) {
        double dist = proximityRead(SERVO_RIGHT);

        // If less than 1 second has passed
        if ((getTime() - clock) < 1000UL) {
            // follow the wall on the right
            followWall(percentSpeed, distanceThreshold, MOTOR_RIGHT);

            if (dist > 12.0 && dist <= 20.0) {
                tilt_right();
            }

            if (dist > 20.0) {
                too_far();
            }

            if (dist < 5.0) {
                too_close();
            }
        } else {
            // STOP motors
            setMotorSpeed(MOTOR_LEFT,  0, FORWARD);
            setMotorSpeed(MOTOR_RIGHT, 0, FORWARD);

            // If there is a wall in front (CENTER)
            double frontDist = proximityRead(SERVO_CENTER);
            if (frontDist < distanceThreshold)
            {
                percentSpeed_left = originalSpeed;
                tilt_left();
                // check right again (as in original code)
                proximityRead(SERVO_RIGHT);
            }

            // Move the servo to the right if no walls are found
            moveServo(SERVO_RIGHT);

            // Reset our 1-second timer
            clock = getTime();
        }

        // For debugging, print the distance
        // (Use printf or some other UART function if you have it)
        // printf("Distance = %f\n", d);

        _delay_ms(50);
    }

    return 0;
}
