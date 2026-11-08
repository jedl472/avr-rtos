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

void debug_USART_print(char *data) {
    uint8_t _sreg = SREG; asm volatile("cli");  //ENTER_CRITICAL
    for (uint16_t i = 0;i < 0xFFFF; i++) {
        if(data[i] != '\0') {
            debug_USART_transmit(data[i]);
        } else {
            break;
        }
    }
    SREG = _sreg;                               //EXIT_CRITICAL
}

void debug_USART_print_int(int value) {
    char buffer[7];  // Enough for -32768 + '\0'
    int i = 0;

    // Handle negative numbers
    if (value < 0) {
        debug_USART_transmit('-');
        value = -value;
    }

    // Special case for zero
    if (value == 0) {
        debug_USART_transmit('0');
        return;
    }

    // Extract digits in reverse order
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    // Send digits in correct order
    while (i > 0) {
        debug_USART_transmit(buffer[--i]);
    }
}

void debug_USART_print_hex(uint16_t value) {
    const char hex[] = "0123456789ABCDEF";

    debug_USART_transmit('0');
    debug_USART_transmit('x');

    for (int i = 3; i >= 0; i--) {
        debug_USART_transmit(hex[(value >> (i * 4)) & 0xF]);
    }
}
