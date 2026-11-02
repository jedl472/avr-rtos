#ifndef TESTS_H
#define TESTS_H

#include "core.h"
#include "queue.h"
#include "debug.h"

// prints all registered tasks to USART
void testcore_tasksetup_print_registered_tasks(void);


// this initializes a task that slowly overwrites its whole stack, testing if kernel panic is thrown at the end of stack 
void testcore_stackprotection_canary_setup(void);

// tries to allocate space on stack with size over task stack size. Should trigger kernel panic 
void testcore_stackprotection_sp_overflow_setup(void);

#endif // !TESTS_H
