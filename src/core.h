#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "config.h"


#define coreENTER_CRITICAL() uint8_t _sreg = SREG; asm volatile("cli")
#define coreEXIT_CRITICAL() SREG = _sreg

#define UNUSED(x) (void)(x) // to supress -Wunused-parameter with creating tasks

#define coreMINIMUM_STACK_SIZE 128 // TODO find out this value fr
#define coreIDLE_TASK_PRIORITY 127
#define coreSTACK_CANARY_VALUE 0x67


// reserved values: 127 - coreIDLE_TASK_PRIORITY
typedef int8_t task_priority_t;
typedef uint16_t tick_counter_t;

// incremented by tick_isr - for debug purposes only
extern tick_counter_t os_ticks;

// Interface for queue-like structures to use for unblocking tasks.
// If not zero, task gets woken up.
// Defined as struct for later adding of parameters.
typedef struct flag {
    uint8_t *count;
} flag_t;

// State of infividual tasks used by the schedulerl.
typedef enum task_state {
    RUNNING,    // currently running
    READY,      // ready to run
    SUSPENDED,  // sleeping for ticks, gets woken up by the scheduler
    BLOCKED,    // waiting for an event, gets woken up by an event
    FINISHED    // effectivelly marked for deletion, unused
} task_state_t;

typedef struct tcb {
    uint8_t *sp;                // stack pointer
    task_state_t state;         // enum for states of the task used by scheduler
    uint8_t id;                 // unused
    task_priority_t priority;   // scheduling priority, 0 is the highest
    
    tick_counter_t ticks_remaining_suspended;   // ticks remaining for SUSPENDED tasks
    flag_t *waiting_on_flag;                    // flag used by the scheduler to wake BLOCKED tasks 

    uint16_t stack_size;
    uint8_t *stack_base;

} tcb_t;


extern volatile tcb_t *current_tcb;


__attribute__((naked)) void task_yield(void);

void task_create(tcb_t *t, task_priority_t priority, uint8_t *stack, 
                    uint16_t size, void (*fn)(void *), void *arg);

void task_suspend_ticks(tick_counter_t ticks);

void os_start(void);

void debug_register_task(tcb_t *task, uint8_t pos);

void task_register(tcb_t *tcb);


extern tcb_t *registered_tasks[configMAX_TASKS];  // TODO remove

typedef enum kp_type {
    //core errors
    STACK_OVERFLOW, // TODO
    EXCEEDED_MAX_TASKS, // user error - means user tried to register more tasks than configMAX_TASKS
    CORRUPTED_IDLE_TASK, // kernel error - means scheduler did not find suitable task to run

    //queue errors
    QUEUE_OVERFLOW, // user error - tried to queue_add on a full queue
    POP_ON_EMPTY_QUEUE // kernel error - called queue_pop on an empty queue
} kp_type_t;

void throw_kp(kp_type_t kp_type);

#endif // !CORE_H

