// #include "debug.h"
// #include "tests/queue.h"
// #include "tests/heap.h"
#include "tests/core.h"

int main(void)
{
    // debug_USART_init(CALCULATE_UBRR(9600));
    // debug_USART_print("========== main start ===========\n");
    
    tests_core_sp_overflow();
}
