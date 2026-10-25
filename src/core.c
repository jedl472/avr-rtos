#include "core.h"

// ----------------------------------------
// macros
// ----------------------------------------

#define SAVE_CONTEXT()                      \
asm volatile(                               \
    "push r0            \n\t"               \
    "in   r0, __SREG__  \n\t"               \
    "cli                \n\t"               \
    "push r0            \n\t"               \
    "push r1            \n\t"               \
    "clr  r1            \n\t" /* C needs r1 == 0 */ \
    "push r2  \n\t push r3  \n\t push r4  \n\t push r5  \n\t" \
    "push r6  \n\t push r7  \n\t push r8  \n\t push r9  \n\t" \
    "push r10 \n\t push r11 \n\t push r12 \n\t push r13 \n\t" \
    "push r14 \n\t push r15 \n\t push r16 \n\t push r17 \n\t" \
    "push r18 \n\t push r19 \n\t push r20 \n\t push r21 \n\t" \
    "push r22 \n\t push r23 \n\t push r24 \n\t push r25 \n\t" \
    "push r26 \n\t push r27 \n\t push r28 \n\t push r29 \n\t" \
    "push r30 \n\t push r31 \n\t"           \
    "lds  r26, current_tcb   \n\t"          \
    "lds  r27, current_tcb+1 \n\t"          \
    "in   r0, __SP_L__ \n\t"                \
    "st   X+, r0       \n\t"                \
    "in   r0, __SP_H__ \n\t"                \
    "st   X+, r0       \n\t"                \
)

#define RESTORE_CONTEXT()                   \
asm volatile(                               \
    "lds  r26, current_tcb   \n\t"          \
    "lds  r27, current_tcb+1 \n\t"          \
    "ld   r28, X+      \n\t"                \
    "out  __SP_L__, r28 \n\t"               \
    "ld   r28, X+      \n\t"                \
    "out  __SP_H__, r28 \n\t"               \
    "pop r31 \n\t pop r30 \n\t pop r29 \n\t pop r28 \n\t" \
    "pop r27 \n\t pop r26 \n\t pop r25 \n\t pop r24 \n\t" \
    "pop r23 \n\t pop r22 \n\t pop r21 \n\t pop r20 \n\t" \
    "pop r19 \n\t pop r18 \n\t pop r17 \n\t pop r16 \n\t" \
    "pop r15 \n\t pop r14 \n\t pop r13 \n\t pop r12 \n\t" \
    "pop r11 \n\t pop r10 \n\t pop r9  \n\t pop r8  \n\t" \
    "pop r7  \n\t pop r6  \n\t pop r5  \n\t pop r4  \n\t" \
    "pop r3  \n\t pop r2  \n\t pop r1  \n\t"  \
    "pop r0  \n\t"                          \
    "out __SREG__, r0 \n\t"                 \
    "pop r0  \n\t"                          \
)

// ----------------------------------------
// globals
// ----------------------------------------

tcb_t *current_tcb;

// array of all registered tasks in the system. MUST be sorted by priority from highest to due to tick ISR having to have small footprint
tcb_t *registered_tasks[configMAX_TASKS] = {NULL};


// ----------------------------------------
// scheduler
// ----------------------------------------

void tick_isr_init(void) { //inits timer2
    TCCR2A = (1 << WGM21);               // CTC mode
    TCCR2B = (1 << CS22);                // prescaler = 64
    OCR2A  = 249;                        // 16MHz / 64 / 250 = 1000 Hz
    TIMSK2 = (1 << OCIE2A);              // enable compare match A interrupt

    asm volatile("sei");
}

/* prerequisities for the scheduler to work: 
        1. registered_tasks is sorted from highest priority
*/

tcb_t *find_next_task() {
    task_priority_t current_priority;

    if (current_tcb == NULL) {
        current_priority = 127; 
    } else {
        current_priority = current_tcb->priority;
    }
    
    
    int8_t highest_ready_priority = -1;
    
    // -------------- find the highest READY priority --------------------
    for(int i = 0; i < configMAX_TASKS; i++) {
        if(registered_tasks[i] != NULL && registered_tasks[i]->state == READY) {
            highest_ready_priority = registered_tasks[i]->priority;
            break;
        }        
    }

    // -------------- if no tasks READY -> keep current_tcb  ------------
    if(highest_ready_priority == -1) {
        if(current_tcb == NULL) return NULL;
        return current_tcb;
    }
    
    // -------------- if higher priority task READY, it ought to run -----
    if(highest_ready_priority < current_priority) {
        for(int i = 0; i < configMAX_TASKS; i++) {
            if(registered_tasks[i] != NULL &&
               registered_tasks[i]->state == READY &&
               registered_tasks[i]->priority == highest_ready_priority) {
                
                return registered_tasks[i];
            }
        }        
    }
    
    // -------------- round robin amongst highest priority tasks --------
    if(highest_ready_priority == current_priority) {
        //find currently running task index
        int8_t currently_running_task_index;
        for(int i = 1; i <= configMAX_TASKS; i++) {
            if(registered_tasks[i]->state == RUNNING) {
                currently_running_task_index = i;
                break;
            }
        }


        int8_t i;
        for(int offset = 1; offset <= configMAX_TASKS; offset++) {
            i = (currently_running_task_index + offset) % configMAX_TASKS;
            
            if(registered_tasks[i] != NULL &&
               registered_tasks[i]->state == READY &&
               registered_tasks[i]->priority == current_priority) {
            
                return registered_tasks[i];
            }
        
        }
    }

    // -------------- else run highest priority READY task found --------
    for(int i = 0; i < configMAX_TASKS; i++) {
        if(registered_tasks[i] != NULL && registered_tasks[i]->state == READY) {
            
            return registered_tasks[i];
        }
    }
}

ISR(TIMER2_COMPA_vect, ISR_NAKED) { // TODOp0 pridat respektovani kriticke casti
    SAVE_CONTEXT();
    // system_ticks++; // time keeping for SUSPENDED tasks
    // wake_expired_delays(); // set SUSPENDED tasks to READY
    tcb_t *next_task = find_next_task();
    if(next_task != 0) {
        current_tcb = next_task;
    } else {
        // TODOp0 sleep
    }

    RESTORE_CONTEXT();
    asm volatile("reti");
}



// ----------------------------------------
// functions
// ----------------------------------------

// void context_switch(tcb_t *task) {
//
// }
//
// void task_yield(void) {
//     currently_running_task = currently_running_task->next_task;
//     context_switch(currently_running_task);
// }

