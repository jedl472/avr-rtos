#ifndef DEBUG_H
#define DEBUG_H


#include <avr/io.h>
#include <util/delay.h>


void debug_blink_lockout(void);

void debug_USART_init(unsigned int ubbr);
void debug_USART_transmit(unsigned char data);

void debug_USART_print(char *data);

#endif // !DEBUG_H
