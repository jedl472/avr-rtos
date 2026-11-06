#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <string.h>

#include "config.h"
#include "core.h"

// macro that will calculate size of queue buffer to allocate - exists to compensate for popped_item_buffer
#define QUEUE_BUFFER_SIZE(item_size, capacity) \
    ((size_t)(item_size) * (size_t)(capacity + 1))

typedef struct queue {
    uint8_t *head;      // pointer to the begining of queue storage area
    size_t item_size;   // size of item in byte
    uint8_t capacity;   // max number of items
    uint8_t *write_to;  // pointer of the next free space in the queue
    uint8_t count;      // number of items in the queue
    uint8_t *popped_item_buffer; // buffer where popped elements go
    
    flag_t flag;        // flag used by the scheduler to wake up tasks
} queue_t;

#endif // !QUEUE_H
