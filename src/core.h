#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define configMAX_TASKS

typedef enum task_state {
    RUNNING,    // currently running
    READY,      // ready to run
    SUSPENDED,  // sleeping for ticks
    BLOCKED     // waiting for an event
} task_state_t;

typedef struct tcb {
    char *sp;
    task_state_t state;
    uint8_t id;
    uint8_t priority;   // 0 is the highest
    
    char *alocated_memory;
    uint16_t alocated_memory_size;

} tcb_t;




#endif // !CORE_H

