#include "ultrasonic.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

void setupUltrasonic(void) {
    // Trigger pin as output
    DDRB |= (1 << TRIGGER_PIN);
    // Clear trigger
    PORTB &= ~(1 << TRIGGER_PIN);

    // Echo pin as input
    DDRB &= ~(1 << ECHO_PIN);

    // If needed, configure Timer1 for counting the echo duration
    // (We also use Timer1 for servo, so the measureDistance logic
    //  might rely on reading TCNT1 directly. See measureDistance() below.)
}

double measureDistance(void) {
    double duration, distance;

    // Trigger the sensor
    PORTB |= (1 << TRIGGER_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIGGER_PIN);

    // Wait for echo start
    while (!(PINB & (1 << ECHO_PIN))) {
    }

    // Reset Timer1
    TCNT1 = 0;

    // Wait while echo is high
    while (PINB & (1 << ECHO_PIN)) {
        // optionally add a timeout check
    }

    // The raw count from Timer1
    duration = (double)TCNT1;

    // Convert to distance.
    // “duration * 0.02700787” depends on prescaler, clock, speed of sound, etc.
    distance = duration * 0.02700787;  // example factor for your prescaler

    return distance;
}

// -----------------------------------------------------------------------------
// Servo Setup (Timer1) & Move
// -----------------------------------------------------------------------------
void setupServo(void)
{
    // SERVO pin as output
    DDRB |= (1 << SERVO);

    /*
       Configure Timer1 for Fast PWM, Mode 15:
         WGM13=1, WGM12=1, WGM11=1, WGM10=1
         => TOP = OCR1A, compare on OCR1B
         => 10-bit or 16-bit depends on how you set OCR1A
    */
    TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12);

    // Prescaler 64 => ~50Hz for servo if OCR1A = 4999
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Period = 20ms -> (16MHz / 64) = 250kHz => 20ms = 5000 counts
    OCR1A = 4999; // 20ms period
    OCR1B = SERVO_CENTER; // initial servo position
}

void moveServo(int pulseWidth) {
    OCR1B = pulseWidth;
}

void ledOn(char led)
{
    if (led == 'r') {
        PORTB |= (1 << RED_LED);
    } else if (led == 'g') {
        PORTB |= (1 << GREEN_LED);
    }
}

void ledOff(char led)
{
    if (led == 'r') {
        PORTB &= ~(1 << RED_LED);
    } else if (led == 'g') {
        PORTB &= ~(1 << GREEN_LED);
    }
}

void setupLed(void)
{
    DDRB |= (1 << RED_LED) | (1 << GREEN_LED);
}

// -----------------------------------------------------------------------------
// Basic Proximity Read
// -----------------------------------------------------------------------------
double proximityRead(int direction)
{
    moveServo(direction);
    _delay_ms(500);

    // flush reading
    (void)measureDistance();
    _delay_ms(60);

    return measureDistance();
}

// -----------------------------------------------------------------------------
// Proximity Read with LED feedback
// -----------------------------------------------------------------------------
void proximityReadLed(int direction, double threshold)
{
    moveServo(direction);
    _delay_ms(500);

    (void)measureDistance(); // flush
    _delay_ms(60);

    double distance = measureDistance();

    if (distance < threshold) {
        switch (direction) {
            case SERVO_LEFT:
                ledOn('g');
                ledOff('r');
                break;
            case SERVO_RIGHT:
                ledOn('r');
                ledOff('g');
                break;
            case SERVO_CENTER:
                ledOn('g');
                ledOn('r');
                break;
        }
    } else {
        ledOff('r');
        ledOff('g');
    }
}

// -----------------------------------------------------------------------------
// Additional Demo Functions
// -----------------------------------------------------------------------------
void suga(void)
{
    // Example that uses proximityReadLed
    proximityReadLed(SERVO_LEFT, MAX);
    // If you have printf via UART, you could do:
    // printf("Left: %.2f\n", proximityRead(LEFT));
    _delay_ms(500);

    proximityReadLed(SERVO_RIGHT, MAX);
    // printf("Right: %.2f\n", proximityRead(RIGHT));
    _delay_ms(500);

    proximityReadLed(SERVO_CENTER, MAX);
    // printf("Center: %.2f\n", proximityRead(CENTER));
    _delay_ms(500);
}

void center(void)
{
    proximityReadLed(SERVO_CENTER, MAX);
    // printf("Center: %.2f inches\n", proximityRead(CENTER));
    _delay_ms(3000);
}
