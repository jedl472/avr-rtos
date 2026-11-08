#include "queue.h"
#include <stdint.h>


static uint8_t test_queue_buffer[QUEUE_BUFFER_SIZE(sizeof(uint8_t), 8)];
static queue_t test_queue;
queue_t *tq_handle = &test_queue;

void task_sender(void *args) {
    uint8_t i = 0;

    while(1) {
        queue_add(tq_handle, &i);
        
        task_suspend_ticks(500);
    }

}

void task_receiver(void *args) {
    uint8_t qpop;
    while (1) {
        suspend_until_queue_receive(tq_handle);
        qpop = *(tq_handle->popped_item_buffer);
        
        debug_USART_print_int(qpop);
    }
}

void tests_queue_runall() {
    debug_USART_init(CALCULATE_UBRR(9600));
    
    queue_init(tq_handle, test_queue_buffer, sizeof(uint8_t), 8);
    
    
    
}
