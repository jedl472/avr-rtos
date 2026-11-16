#ifndef TESTS_H
#define TESTS_H


#include "../debug.h"

#define TEST_INIT() debug_USART_init(CALCULATE_UBRR(9600))

#define tests_print_str(str) debug_USART_print(str) 
#define tests_print_dec(dec) debug_USART_print_int(dec) 
#define tests_print_hex(hex) debug_USART_print_hex(hex) 


static int tests_run = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond, msg) do { \
    tests_run++;                    \
    if(!(cond)) {                   \
        tests_print_str("[FAIL] "); \
        tests_print_str(msg);       \
        tests_print_str("\n");      \
        tests_failed++;             \
    } else {                        \
        tests_print_str("[PASS] "); \
        tests_print_str(msg);       \
        tests_print_str("\n");      \
    }                               \
} while(0)                  


#endif 
