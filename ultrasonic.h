#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <avr/io.h>

/* Servo angle definitions (Timer1-based).
   Adjust these to your actual servo angles / counts. */
#define SERVO_RIGHT   150   // ~ -90 deg
#define SERVO_LEFT    625   // ~ +90 deg
#define SERVO_CENTER  375   //   0 deg
#define MAX     30    // used for "proximityReadLed" threshold

/* Pin definitions for ultrasonic, LED, servo, etc. */
#define TRIGGER_PIN PB4
#define ECHO_PIN    PB5
#define SERVO       PB2
#define RED_LED     PB1
#define GREEN_LED   PB0

/* Setup ultrasonic and servo. */
void setupUltrasonic(void);
double measureDistance(void);
void setupServo(void);
void moveServo(int pulseWidth);

/* LED control */
void ledOn(char led);
void ledOff(char led);
void setupLed(void);

/* Basic distance reading with servo rotation */
double proximityRead(int direction);

/* Check-off test function with LED feedback */
void proximityReadLed(int direction, double threshold);

/* Additional test functions */
void suga(void);
void center(void);

#endif
