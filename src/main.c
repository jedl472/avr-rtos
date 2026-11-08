#define F_CPU 16000000UL // for 16 mHz nano m328p


#include "debug.h"

#include "core.h"
#include "tests/queue.h"

//
// static tcb_t   blink_task_tcb;
// static uint8_t blink_task_stack[coreMINIMUM_STACK_SIZE];

//
// void blink_task(void *args) {
//     UNUSED(args);
//     while (1) {
//         PORTB |= (1 << PB5);   // LED on
//         task_suspend_ticks(200);
//
//         PORTB &= ~(1 << PB5);  // LED off
//         task_suspend_ticks(200);
//     }
// }

int main(void)
{
    tests_queue_runall();
    // //debug init
    // DDRB |= (1 << PB5);  // PB0 output
    // debug_USART_init(CALCULATE_UBRR(9600));
    //
    // //
    // // debug_USART_print("""Hello world xoxo ver2\n""");
    // // debug_USART_print("blink stack canary address: ");
    // // debug_USART_print_hex((uintptr_t)blink_task_stack);
    // // debug_USART_print("\n");
    // task_create(&blink_task_tcb, 0, blink_task_stack, sizeof(blink_task_stack), blink_task, NULL);
    // task_register(&blink_task_tcb);
    //
    //
    //
    // os_start();
        
}
