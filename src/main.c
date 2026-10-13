#define F_CPU 16000000UL // for 16 mHz nano m328p

#include "debug.h"
#define BAUD 9600
#define UBRR F_CPU/16/BAUD-1

int main(void)
{
    debug_USART_init(UBRR);
    debug_USART_print("""Hello world xoxo""");
    debug_blink_lockout();
}
