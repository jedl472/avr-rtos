#include "core.h"

// ----------------------------------------
// macros
// ----------------------------------------


#define SAVE_CONTEXT()                      \
asm volatile(                               \
    "push r0            \n\t"               \
    "in   r0, __SREG__  \n\t"               \
    "cli                \n\t"               \
    "set                \n\t"               \
    "bld  r0, 7         \n\t" /* Ensure saved SREG keeps I-bit enabled */ \
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
// "private" implicit declarations
// ----------------------------------------

void check_stack_bounds(tcb_t *task);


// ----------------------------------------
// globals
// ----------------------------------------

volatile tcb_t *current_tcb;

// array of all registered tasks in the system. MUST be sorted by priority from highest to due to tick ISR having to have small footprint
tcb_t *registered_tasks[configMAX_TASKS] = {NULL};



// ----------------------------------------
// idle task
// ----------------------------------------

static tcb_t idle_task_tcb;
static uint8_t idle_task_stack[coreMINIMUM_STACK_SIZE];

void idle_task(void *args) {
    UNUSED(args);
    while(1) {}
}



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

tcb_t *find_next_task(void) {
    // ------------------------ find the current RUNNING task ----------------------
    int8_t running_index = -1;
    for (int i = 0; i < configMAX_TASKS; i++) {
        if (registered_tasks[i] == current_tcb && current_tcb != NULL) {
            running_index = i;
            break;
        }
    }

    // ------------------------ find highest priority RUNNING or READY task --------
    int8_t best_priority = 127;
    for (int i = 0; i < configMAX_TASKS; i++) {
        if (registered_tasks[i] != NULL &&
           (registered_tasks[i]->state == READY || registered_tasks[i]->state == RUNNING)) {
            if (registered_tasks[i]->priority < best_priority) {
                best_priority = registered_tasks[i]->priority;
            }
        }
    }

    // ------------------------ round robin after currently running task -----------
    int start = (running_index >= 0) ? (running_index + 1) : 0;
    for (int offset = 0; offset < configMAX_TASKS; offset++) {
        int i = (start + offset) % configMAX_TASKS;
        if (registered_tasks[i] != NULL &&
           (registered_tasks[i]->state == READY || registered_tasks[i]->state == RUNNING) &&
            registered_tasks[i]->priority == best_priority) {
            return registered_tasks[i];
        }
    }

    // ------------------------ if no task is ready, return idle taks --------------
    return &idle_task_tcb;
}

void scheduler(void) {
    tcb_t *next_task = find_next_task();
    if(next_task != NULL) {
        if(current_tcb != NULL && current_tcb->state == RUNNING) {
            current_tcb->state = READY;
        }
        current_tcb = next_task;
        current_tcb->state = RUNNING;
    } else {
        // shouldn't be reachable, so being here means scheduler did not find READY idle tas
        throw_kp(CORRUPTED_IDLE_TASK);
    }

}

void update_suspended_tasks(void) {
    for(int i = 0; i < configMAX_TASKS; i++) {
        if(registered_tasks[i] == NULL) break;
        if(registered_tasks[i]->state == SUSPENDED) {
            if (registered_tasks[i]->ticks_remaining_suspended > 0) {
                registered_tasks[i]->ticks_remaining_suspended--;
            }
            if (registered_tasks[i]->ticks_remaining_suspended == 0) {
                registered_tasks[i]->state = READY;
            }
        } else if(registered_tasks[i]->state == BLOCKED) {
            if(*(registered_tasks[i]->waiting_on_flag->count) != 0) {
                registered_tasks[i]->state = READY;
            }
        }
    }
}

ISR(TIMER2_COMPA_vect, ISR_NAKED) { // TODOp0 pridat respektovani kriticke casti
    SAVE_CONTEXT();
    check_stack_bounds((tcb_t*)current_tcb);
    // system_ticks++;
    update_suspended_tasks();
    scheduler();
    RESTORE_CONTEXT();
    asm volatile("reti");
}


// ----------------------------------------
// core utilities
// ----------------------------------------

__attribute__((naked)) void start_first_task(void) {
    RESTORE_CONTEXT();
    asm volatile("ret");
}

void os_start(void) {
    task_create(&idle_task_tcb, coreIDLE_TASK_PRIORITY, idle_task_stack, sizeof idle_task_stack, idle_task, NULL);    
    task_register(&idle_task_tcb);

    scheduler();
    tick_isr_init();
    start_first_task();
}

void task_suspend_ticks(tick_counter_t ticks) {
    current_tcb->state = SUSPENDED;
    current_tcb->ticks_remaining_suspended = ticks;

    task_yield();
}

void task_exit(void) {
    current_tcb->state = FINISHED;      
    for (;;) {
        task_yield();                      
    }
}

void debug_register_task(tcb_t *task, uint8_t pos) {
    registered_tasks[pos] = task;  // throws -Warray-bounds and that is perfectly fine
}

void check_stack_bounds(tcb_t *task) {
    if(*(task->stack_base) != coreSTACK_CANARY_VALUE) {
        throw_kp(STACK_OVERFLOW);
    }

    if(task->sp <= task->stack_base || (uintptr_t)current_tcb->sp > RAMEND) {
        throw_kp(STACK_OVERFLOW);
    }
}

// ----------------------------------------
// task utilities
// ----------------------------------------

__attribute__((naked)) void task_yield(void) {
    SAVE_CONTEXT();
    scheduler();
    RESTORE_CONTEXT();
    asm volatile("ret");
}

void task_create(tcb_t *tcb, task_priority_t priority, uint8_t *stack, uint16_t size,
                 void (*fn)(void *), void *arg) {
    
    uint8_t *sp = stack + size - 1;     // highest box of the stack array

    // if the task function ever returns, land in task_exit
    uint16_t exit_addr = (uint16_t)task_exit;
    *sp-- = exit_addr & 0xFF;
    *sp-- = exit_addr >> 8;

    // 1-2: where the task starts (what reti will load into the program counter)
    uint16_t start = (uint16_t)fn;
    *sp-- = start & 0xFF;               // low byte, higher address
    *sp-- = start >> 8;                 // high byte, lower address

    *sp-- = 0x00;                       // 3: r0
    *sp-- = 0x80;                       // 4: SREG, bit 7 = interrupts enabled
    *sp-- = 0x00;                       // 5: r1, must be zero for C code

    for (uint8_t r = 2; r <= 31; r++) { // 6: r2 ... r31
        uint16_t a = (uint16_t)arg;
        if      (r == 24) *sp-- = a & 0xFF;   // first argument, low byte
        else if (r == 25) *sp-- = a >> 8;     // first argument, high byte
        else              *sp-- = 0x00;
    }

    tcb->sp = sp;                         // next free box, as a real stack pointer would be
    tcb->priority = priority;
    tcb->state = READY;
    tcb->ticks_remaining_suspended = 0;

    tcb->stack_size = size;
    tcb->stack_base = stack;

    *stack = coreSTACK_CANARY_VALUE; // stack canary setup - the only place where that address should be written to
}


void task_register(tcb_t *tcb) {
    // debug_USART_print("registering task of priority: ");
    // debug_USART_transmit(tcb->priority + '0');
    // debug_USART_print("\n");
    if (registered_tasks[configMAX_TASKS - 1] != NULL) throw_kp(EXCEEDED_MAX_TASKS);
    
    if (registered_tasks[0] == NULL) {
        registered_tasks[0] = tcb;
        return;
    }

    uint8_t pos = 0; // initialized to 0 to supress -Wmaybe-uninitialized
    for(int i = 0; i < configMAX_TASKS; i++) {
        if(registered_tasks[i] == NULL || registered_tasks[i]->priority >= tcb->priority) {
            pos = i;
            break;
        }
    }

    for(int i = configMAX_TASKS - 1; i >= pos; i--) {
        if(registered_tasks[i] != NULL) registered_tasks[i + 1] = registered_tasks[i];
    }

    registered_tasks[pos] = tcb;
}

// ----------------------------------------
// error handling
// ----------------------------------------

void throw_kp(kp_type_t kp_type) {
    // send out the error somehow
    // UNUSED(kp_type);
    // or not
    
    debug_USART_print("KERNEL PANIC!\nerror code: ");
    debug_USART_transmit(kp_type + '0');
    debug_USART_transmit('\n');
    

    asm volatile("cli");  // disable interrupts    

    while(1) {}
}
