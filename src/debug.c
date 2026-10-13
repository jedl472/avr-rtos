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

// All usart code pulled from atmega238p datasheet

void debug_USART_init(unsigned int ubbr) {
    // set baud rate
    UBRR0H = (unsigned char)(ubbr>>8);
    UBRR0L = (unsigned char)ubbr;
    // enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // set frame format: 8data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void debug_USART_transmit(unsigned char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1<<UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;

}

void debug_USART_print(unsigned char *data) {
    for (uint16_t i = 0;i < 0xFFFF; i++) {
        if(data[i] != '\0') {
            debug_USART_transmit(data[i]);
        } else {
            break;
        }
    }
}
