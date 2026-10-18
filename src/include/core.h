#ifndef CORE_H
#define CORE_H

#include <stdint.h>



typedef struct tcb {
    uint8_t *sp;
    struct tcb *next_task;
} tcb_t;

extern tcb_t *currently_running_task;

void task_yield(void);



#endif // !CORE_H

