#include "tests.h"

// ------------------------------------
// CORE tests
// ------------------------------------
 
// ----------------------------- task setup

void testcore_tasksetup_print_registered_tasks(void) {
    for (int i = 0; i < configMAX_TASKS; i++) {
       if(registered_tasks[i] == NULL) debug_USART_transmit('n');
       else debug_USART_transmit(registered_tasks[i]->priority + '0');

       debug_USART_print("\n");
    }    
}

// ----------------------------- stack protection

static uint8_t test_core_task_stack[coreMINIMUM_STACK_SIZE];

static tcb_t   testcore_stackprotection_canary_overwriter_task_tcb;
void testcore_stackprotection_canary_overwriter_task(void *args) {
    UNUSED(args);

    int8_t random_value = 0x67;
    int8_t *addr = &random_value;
    uint16_t index = 0;
    
    debug_USART_print("canary addr: ");
    debug_USART_print_hex((uintptr_t)&test_core_task_stack[0]);
    debug_USART_print("\n");
    
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

void testcore_stackprotection_canary_setup(void) {
    debug_USART_print("------- Testing core stack canary overwriting -------\n");

    task_create(&testcore_stackprotection_canary_overwriter_task_tcb,
                0, 
                test_core_task_stack, 
                sizeof(test_core_task_stack), 
                testcore_stackprotection_canary_overwriter_task, 
                NULL);
    task_register(&testcore_stackprotection_canary_overwriter_task_tcb);

    os_start();
}


static tcb_t   testcore_stackprotection_sp_overflow_task_tcb;
void testcore_stackprotection_sp_overflow_task(void *args) {
    UNUSED(args);

    volatile int8_t ahoj[coreMINIMUM_STACK_SIZE * 2 ];
    ahoj[0] = 4; // so this wont get optimized out

    while(1) {
    }
}

void testcore_stackprotection_sp_overflow_setup(void) {
    debug_USART_print("------- Testing core sp overflow -------\n");

    task_create(&testcore_stackprotection_sp_overflow_task_tcb,
                0, 
                test_core_task_stack, 
                sizeof(test_core_task_stack), 
                testcore_stackprotection_sp_overflow_task, 
                NULL);
    task_register(&testcore_stackprotection_sp_overflow_task_tcb);

    os_start();
}
