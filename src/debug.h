#ifndef DEBUG_H
#define DEBUG_H


#include <avr/io.h>
#include <util/delay.h>

#define CALCULATE_UBRR(baud_rate) ((F_CPU / 16 / (baud_rate)) - 1)

void debug_blink_lockout(void);

void debug_USART_init(unsigned int ubbr);
void debug_USART_transmit(unsigned char data);

void debug_USART_print(char *data);
void debug_USART_print_int(int value);

void debug_USART_print_hex(uint16_t value);

#endif // !DEBUG_H
