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
