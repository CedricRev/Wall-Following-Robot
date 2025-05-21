#include "motors.h"
#include "ultrasonic.h"  // for moveServo(), measureDistance(), etc.
#include <util/delay.h>  // for _delay_ms()
#include <stdbool.h>     // for bool (optional if you use it)
#include <stdio.h>       // for printf (if desired for debug)

void enableMotors(void) {
    DDRD |= (1 << LEFT_CONTROL);   // PD2
    DDRD |= (1 << LEFT_PWM);       // PD5
    DDRD |= (1 << RIGHT_CONTROL);  // PD4
    DDRD |= (1 << RIGHT_PWM);      // PD6
}

void setupMotors(void) {
    enableMotors();

    // Configure Timer0 for Fast PWM
    // WGM01=1 and WGM00=1 => Fast PWM
    TCCR0A = (1 << WGM01) | (1 << WGM00);

    // Non-inverting PWM on OC0A (PD6) and OC0B (PD5)
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    // Prescaler = 64 => ~976.56 Hz @ 16 MHz
    TCCR0B = (1 << CS01) | (1 << CS00);

    // Initialize PWM duty cycles to 0
    OCR0A = 0; // Right motor (PD6)
    OCR0B = 0; // Left motor (PD5)
}

// -----------------------------------------------------------------------------
// Set Motor Speed
// -----------------------------------------------------------------------------

void setMotorSpeed(int motor, double speed, int direction)
{
    /* speed is assumed to be from 0.0 to 1.0 (or up to something),
       then multiplied by MOTOR_TOP_SPEED. Adjust as necessary. */

    uint8_t pwm_value = (uint8_t)(speed * MOTOR_TOP_SPEED);
    if (pwm_value > 255) {
        pwm_value = 255; // clamp
    }

    if (motor == MOTOR_LEFT) {
        OCR0B = pwm_value; // PD5
        if (direction == FORWARD) {
            PORTD |= (1 << LEFT_CONTROL);   // PD2 = HIGH => forward
        } else {
            PORTD &= ~(1 << LEFT_CONTROL);  // PD2 = LOW => backward
        }
    }
    else if (motor == MOTOR_RIGHT) {
        OCR0A = pwm_value; // PD6
        if (direction == FORWARD) {
            PORTD &= ~(1 << RIGHT_CONTROL); // PD4 = LOW => forward
        } else {
            PORTD |= (1 << RIGHT_CONTROL);  // PD4 = HIGH => backward
        }
    }
}

// -----------------------------------------------------------------------------
// followWall
// -----------------------------------------------------------------------------
void followWall(double percentPower, double maintainDistance, int side)
{
    /*
      1) Move servo to side (MOTOR_LEFT or MOTOR_RIGHT)
      2) Delay so servo has time to get there
      3) Measure distance
      4) Adjust motor speeds
    */

    moveServo(side);
    _delay_ms(100);

    (void)measureDistance(); // "flush" reading if needed
    _delay_ms(60);

    double distance = measureDistance();
    if (distance > 30) {
        // Filter out very large readings
        distance = maintainDistance;
    }

    // If side=MOTOR_RIGHT => ratio is (distance / maintainDistance) for left motor
    // and (maintainDistance / distance) for right motor
    if (side == MOTOR_RIGHT) {
        setMotorSpeed(MOTOR_LEFT,  (percentPower * (distance / maintainDistance)),  FORWARD);
        setMotorSpeed(MOTOR_RIGHT, (percentPower * (maintainDistance / distance)), FORWARD);
    }
    else if (side == MOTOR_LEFT) {
        setMotorSpeed(MOTOR_LEFT,  (percentPower * (distance / maintainDistance)),  FORWARD);
        setMotorSpeed(MOTOR_RIGHT, (percentPower * (maintainDistance / distance)), FORWARD);
    }

    _delay_ms(60);
}
