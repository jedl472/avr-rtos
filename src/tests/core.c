#include "core.h"
#include "tests.h"

// ============================================================================
// blink test
// ============================================================================

static task_handle_t blink_task_handle;

void blink_task(void *args) {
    UNUSED(args);
    while (1) {
        PORTB |= (1 << PB5);   // LED on
        task_suspend_ticks(200);

        PORTB &= ~(1 << PB5);  // LED off
        task_suspend_ticks(200);
    }
}

void tests_core_blink() {
    TEST_INIT();

    DDRB |= (1 << PB5);  // PB0 output
    blink_task_handle = task_create(0, coreMINIMUM_STACK_SIZE, blink_task, NULL);
    TEST_ASSERT(1, "T1.1: Created blink task without crashing.");

    TEST_ASSERT(1, "T1.2: Calling os_start, onboard LED should start blinking now.");
    os_start();

}


// ============================================================================
// stack protection
// ============================================================================

void canary_overwriter_task(void *args) {
    UNUSED(args);

    int8_t random_value = 0x67;
    int8_t *addr = &random_value;
    uint16_t index = 0;
    
    // debug_USART_print("canary addr: ");
    // debug_USART_print_hex((uintptr_t)&test_core_task_stack[0]);
    // debug_USART_print("\n");
    
    while(1) {
        task_suspend_ticks(10);
        
        int8_t *target = addr - index;
    
        debug_USART_print_int(index);
        debug_USART_print(" - ");
        debug_USART_print_hex((uintptr_t)target);
        debug_USART_print("\n");

        *target = coreSTACK_CANARY_VALUE + 1;
        index++;
    }
}

void tests_core_canary(void) {
    TEST_INIT();
    TEST_ASSERT(1, "T1.1: Kernel panic 0 (STACK_OVERFLOW) should be thrown, because of overwriten canary.");

    task_create(0,
                coreMINIMUM_STACK_SIZE, 
                canary_overwriter_task, 
                NULL);
    os_start();
}

void sp_overflow_task(void *args) {
    UNUSED(args);

    volatile int8_t ahoj[coreMINIMUM_STACK_SIZE * 2 ];
    ahoj[0] = 4; // so this wont get optimized out

    while(1) {
    }
}

void tests_core_sp_overflow(void) {
    TEST_INIT();
    TEST_ASSERT(1, "T1.1: Kernel panic 0 (STACK_OVERFLOW) should be thrown, because sp overflow.");

    task_create(0,
                coreMINIMUM_STACK_SIZE, 
                sp_overflow_task, 
                NULL);

    os_start();
}
