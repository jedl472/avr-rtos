#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>

#include "config.h"
#include "core.h"

typedef int8_t queue_element_t;

typedef struct queue {
    const queue_element_t *head; // pointer to the begining of queue storage area
    queue_element_t *write_to; // next free field
    queue_element_t *popped_element_storage;
    const uint8_t max_capacity;
    uint8_t number_of_elements;
    size_t element_size;
} queue_t;

#endif // !QUEUE_H
