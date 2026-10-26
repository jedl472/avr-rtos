#define F_CPU 16000000UL // for 16 mHz nano m328p


#include "debug.h"
#define BAUD 9600
#define UBRR F_CPU/16/BAUD-1


#include "core.h"



static tcb_t   blink_task_tcb;
static uint8_t blink_task_stack[coreMINIMUM_STACK_SIZE];


void blink_task(void *args) {
    UNUSED(args);
    while (1) {
        PORTB |= (1 << PB5);   // LED on
        task_suspend_ticks(500);

        PORTB &= ~(1 << PB5);  // LED off
        task_suspend_ticks(500);
    }
}


void print_registered_tasks() {
    for (int i = 0; i < configMAX_TASKS; i++) {
       if(registered_tasks[i] == NULL) debug_USART_transmit('n');
       else debug_USART_transmit(registered_tasks[i]->priority + '0');

       debug_USART_print("\n");
    }    
}


int main(void)
{
    //debug init
    DDRB |= (1 << PB5);  // PB0 output
    debug_USART_init(UBRR);
    
            
    debug_USART_print("""Hello world xoxo ver2\n""");
    task_create(&blink_task_tcb, 0, blink_task_stack, sizeof(blink_task_stack), blink_task, NULL);
    task_register(&blink_task_tcb);


    os_start();
}
