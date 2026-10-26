#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>


#define UNUSED(x) (void)(x) // to supress -Wunused-parameter with creating tasks



#define configMAX_TASKS 1
#define coreMINIMUM_STACK_SIZE 128 // TODO find out this value fr
#define coreIDLE_TASK_PRIORITY 127

// reserved values: 127 - coreIDLE_TASK_PRIORITY
typedef int8_t task_priority_t;
typedef uint16_t tick_counter_t;

typedef enum task_state {
    RUNNING,    // currently running
    READY,      // ready to run
    SUSPENDED,  // sleeping for ticks, gets woken up by the scheduler
    BLOCKED,    // waiting for an event, gets woken up by an event
    FINISHED    // effectivelly marked for deletion
} task_state_t;

typedef struct tcb {
    uint8_t *sp;
    task_state_t state;
    uint8_t id; //yet unused
    task_priority_t priority;   // 0 is the highest
    
    tick_counter_t ticks_remaining_suspended;

} tcb_t;

__attribute__((naked)) void task_yield(void);

void task_create(tcb_t *t, task_priority_t priority, uint8_t *stack, 
                    uint16_t size, void (*fn)(void *), void *arg);

void task_suspend_ticks(tick_counter_t ticks);

void os_start(void);

void debug_register_task(tcb_t *task, uint8_t pos);




typedef enum kp_type {
    STACK_OVERFLOW, // TODO
    EXCEEDED_MAX_TASKS, // user error - means user tried to register more tasks than configMAX_TASKS
    CORRUPTED_IDLE_TASK // internal error - means scheduler did not find suitable task to run
} kp_type_t;

void throw_kp(kp_type_t kp_type);

#endif // !CORE_H

