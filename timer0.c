#include "timer0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t ticks = 0;

ISR(TIMER0_COMPA_vect) {
    ticks++;
}

// 16 MHz clock (ATMega328p) and prescaler 64 
// 16MHz/64 = 250kHz (4us per tick).
void setupTimer0(void) {
    OCR0A = 249;
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    TIMSK0 = (1 << OCIE0A);
    sei();
}

uint32_t getTime(void) {
    uint32_t ms;
    cli();
    ms = ticks;
    sei();
    return ms;
}