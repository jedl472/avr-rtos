#include "debug.h"

void debug_blink_lockout(void) {
    DDRB |= (1 << PB5);  // PB0 output

    while (1) {
        PORTB |= (1 << PB5);   // LED on
        _delay_ms(500);

        PORTB &= ~(1 << PB5);  // LED off
        _delay_ms(500);
    }
}
