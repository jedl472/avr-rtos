#include "queue.h"
#include "core.h"
#include "debug.h"

void queue_init(queue_t *q, uint8_t *head, size_t item_size, uint8_t capacity) {
    q->head = head;
    q->item_size = item_size;
    q->capacity = capacity;
    q->write_to = head;
    q->count = 0;

    q->popped_item_buffer = head + (capacity * item_size); 

    q->flag.count = &(q->count);
}

void bind_queue_to_task(queue_t *q, tcb_t *task) {
    task->waiting_on_flag = &(q->flag);
}

void queue_add(queue_t *q, void *item_to_add) {
    #if configVOID_ELEMENTS_ADDED_TO_FULL_QUEUE
        if(q->capacity == q->count) {
            return;
        }
    #else 
        if(q->capacity == q->count) {
            throw_kp(QUEUE_OVERFLOW);
        }
    #endif

    debug_USART_print("queue add called\n");
    coreENTER_CRITICAL();
    
    memcpy((void*)(q->write_to), item_to_add, q->item_size);
    
    q->write_to += q->item_size;
    q->count++;

    coreEXIT_CRITICAL();
}

void *queue_pop(queue_t *q) {
    coreENTER_CRITICAL();    

    if(q->count == 0) throw_kp(POP_ON_EMPTY_QUEUE);
    
    memcpy((void*)(q->popped_item_buffer), q->head, q->item_size);
    memcpy((void*)(q->head), q->head + q->item_size, q->item_size * q->count); // TODO: is the -Wdiscarded-qualifiers fine?
    
    q->count--;
    q->write_to -= q->item_size;
    
    
    return (void*)(q->popped_item_buffer);
    
    coreEXIT_CRITICAL();
}


// TODO remove this accessing current_tcb
void suspend_until_queue_receive(queue_t *q) {
    bind_queue_to_task(q, current_tcb);
    
    current_tcb->state = BLOCKED;
    
    task_yield();
}
