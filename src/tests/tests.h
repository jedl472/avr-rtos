#ifndef TESTS_H
#define TESTS_H

#include <avr/pgmspace.h>
#include "../debug.h"

#define TEST_INIT() debug_USART_init(CALCULATE_UBRR(9600))

#define tests_print_str(str) debug_USART_print(str) 
#define tests_print_dec(dec) debug_USART_print_int(dec) 
#define tests_print_hex(hex) debug_USART_print_hex(hex) 


// Helper that prints a Flash (PROGMEM) string 1 byte at a time using a 2-byte stack buffer
static inline void tests_print_pstr(PGM_P pstr) {
    char buf[2] = {0, '\0'};
    while ((buf[0] = pgm_read_byte(pstr++)) != '\0') {
        tests_print_str(buf);
    }
}

// Helper for regular runtime strings wraps literals in PSTR() automatically
#define tests_print_flash(str_literal) tests_print_pstr(PSTR(str_literal))

static int tests_run = 0;
static int tests_failed = 0;

// Notice how "[FAIL] " msg "\n" concatenates into ONE Flash string at compile time!
#define TEST_ASSERT(cond, msg) do {                     \
    tests_run++;                                        \
    if (!(cond)) {                                      \
        tests_print_pstr(PSTR("[FAIL] " msg "\n"));     \
        tests_failed++;                                 \
    } else {                                            \
        tests_print_pstr(PSTR("[PASS] " msg "\n"));     \
    }                                                   \
} while(0)  


#endif 
