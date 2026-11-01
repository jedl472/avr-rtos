#include "queue.h"

void queue_add(queue_t *queue, queue_element_t *element_to_add) {
    #if configVOID_ELEMENTS_ADDED_TO_FULL_QUEUE
        if(queue->max_capacity == queue->number_of_elements) {
            throw_kp(QUEUE_OVERFLOW);
        }
    #endif

    memcpy((void*)(queue->write_to), element_to_add, queue->element_size);
    
    queue->write_to += queue->element_size;
    queue->number_of_elements++;
}

queue_element_t *queue_pop(queue_t *queue) {
    if(queue->number_of_elements == 0) throw_kp(POP_ON_EMPTY_QUEUE);

    memcpy((void*)(queue->popped_element_storage), queue->head, queue->element_size);
    memcpy((void*)(queue->head), queue->head + queue->element_size, queue->element_size * queue->number_of_elements); // TODO: is the -Wdiscarded-qualifiers fine?
    
    queue->number_of_elements--;
    queue->write_to -= queue->element_size;
    
    
    return queue->popped_element_storage;
}


void suspend_until_queue_receive(queue_t *queue) {
    
}
