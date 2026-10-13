#define F_CPU 16000000UL // for 16 mHz nano m328p

#include "debug.h"


int main(void)
{
    debug_blink_lockout();
}
