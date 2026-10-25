#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define configMAX_TASKS 8

// reserved values: 127 - idle task priority
typedef int8_t task_priority_t;

typedef enum task_state {
    RUNNING,    // currently running
    READY,      // ready to run
    SUSPENDED,  // sleeping for ticks, gets woken up by the scheduler
    BLOCKED     // waiting for an event, gets woken up by an event.
} task_state_t;

typedef struct tcb {
    char *sp;
    task_state_t state;
    uint8_t id;
    task_priority_t priority;   // 0 is the highest
    
    char *alocated_memory;
    uint16_t alocated_memory_size;

} tcb_t;



#endif // !CORE_H

