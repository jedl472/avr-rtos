#include "queue.h"


static uint8_t test_queue_buffer[QUEUE_BUFFER_SIZE(sizeof(uint8_t), 8)];
static queue_t test_queue;

static tcb_t task_sender_tcb;
static tcb_t task_receiver_tcb;
static uint8_t task_sender_stack[coreMINIMUM_STACK_SIZE];
static uint8_t task_receiver_stack[coreMINIMUM_STACK_SIZE];

queue_t *tq_handle = &test_queue;

void task_sender(void *args) {
    uint8_t i = 0;

    while(1) {
        coreENTER_CRITICAL();
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        queue_add(tq_handle, &i);
        coreEXIT_CRITICAL();
        debug_USART_print("sending data to queue\n waiting_on_flag val: ");
        debug_USART_print_int(*task_receiver_tcb.waiting_on_flag->count);
        debug_USART_print("\n");
        i++; 
        task_suspend_ticks(500);

    }

}

void task_receiver(void *args) {
    debug_USART_print("first run of task_receiver\n");
    uint8_t qpop;
    while (1) {
        suspend_until_queue_receive(tq_handle);
        debug_USART_print("task_receiver woke up hoooooooooooooray\n");
        void *_ = queue_pop(tq_handle);
        qpop = *(tq_handle->popped_item_buffer);
        
        debug_USART_print("popped queue value: ");
        debug_USART_print_int(qpop);
        debug_USART_print("\n");
    }
}

void tests_queue_runall() {
    debug_USART_init(CALCULATE_UBRR(9600));

    debug_USART_print("\n------ starting queue test block ------\n");
    
    queue_init(tq_handle, test_queue_buffer, sizeof(uint8_t), 8);
    
    task_create(&task_sender_tcb, 0, task_sender_stack, sizeof(task_sender_stack), task_sender, NULL);
    task_register(&task_sender_tcb);
    task_create(&task_receiver_tcb, 0, task_receiver_stack, sizeof(task_receiver_stack), task_receiver, NULL);
    task_register(&task_receiver_tcb);

    os_start();

    // test_queue.count = 0;
    // task_receiver_tcb.waiting_on_flag = &(test_queue.flag);
    //
    // queue_pop(&test_queue);
    //
    // debug_USART_print_int(*(task_receiver_tcb.waiting_on_flag->count));
    
}
