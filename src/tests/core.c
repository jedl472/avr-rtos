#include "core.h"
#include "tests.h"

// ============================================================================
// blink test
// ============================================================================

static tcb_t   blink_task_tcb;
static uint8_t blink_task_stack[coreMINIMUM_STACK_SIZE];

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
    task_create(&blink_task_tcb, 0, blink_task_stack, sizeof(blink_task_stack), blink_task, NULL);
    TEST_ASSERT(1, "T1.1: Created blink task without crashing.");

    task_register(&blink_task_tcb);
    TEST_ASSERT(1, "T1.2: Registered blink task without crashing.");
    

    TEST_ASSERT(1, "T1.3: Calling os_start, onboard LED should start blinking now.");
    os_start();

}
