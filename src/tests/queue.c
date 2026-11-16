#include "queue.h"

#define TEST_QUEUE_LEN 8
#define RTOS_TEST_ITEMS 5

static uint8_t test_queue_buffer[QUEUE_BUFFER_SIZE(sizeof(uint8_t), TEST_QUEUE_LEN)];
static queue_t test_queue;
static queue_t *tq_handle = &test_queue;

static task_handle_t task_sender_handle;
static task_handle_t task_receiver_handle;

// ============================================================================
// PART 1: Synchronous Unit Tests (Run before os_start)
// ============================================================================

static void reset_test_queue(void) {
    memset(test_queue_buffer, 0, sizeof(test_queue_buffer));
    memset(&test_queue, 0, sizeof(test_queue));
    queue_init(tq_handle, test_queue_buffer, sizeof(uint8_t), TEST_QUEUE_LEN);
}

// Test 1: Initialization and single send/pop
void test_queue_single_send_pop(void) {
    reset_test_queue();

    uint8_t val_in = 42;
    queue_send(tq_handle, &val_in);

    void *res = queue_pop(tq_handle);
    uint8_t val_out = *(tq_handle->popped_item_buffer);

    TEST_ASSERT(res != NULL, "T1.1: queue_pop returns non-NULL after send");
    TEST_ASSERT(val_out == 42, "T1.2: Popped value matches sent value (42)");
}

// Test 2: FIFO ordering and full capacity (8 items)
void test_queue_fifo_order(void) {
    reset_test_queue();

    for (uint8_t i = 0; i < TEST_QUEUE_LEN; i++) {
        uint8_t val = i + 10;
        queue_send(tq_handle, &val);
    }

    uint8_t fifo_ok = 1;
    for (uint8_t i = 0; i < TEST_QUEUE_LEN; i++) {
        queue_pop(tq_handle);
        uint8_t popped = *(tq_handle->popped_item_buffer);
        if (popped != (uint8_t)(i + 10)) {
            fifo_ok = 0;
        }
    }

    TEST_ASSERT(fifo_ok == 1, "T2.1: Queue preserves FIFO order across full capacity");
}

// ============================================================================
// PART 2: Multi-Task RTOS Blocking & Wakeup Tests
// ============================================================================

void task_sender(void *args) {
    (void)args;
    uint8_t i = 1;

    while (i <= RTOS_TEST_ITEMS) {
        // Verify receiver is blocked on this queue's flag before sending
        TEST_ASSERT(task_receiver_handle->waiting_on_flag != NULL,
                    "T4.x: Receiver TCB is waiting on queue flag before send");

        coreENTER_CRITICAL();
        queue_send(tq_handle, &i);
        coreEXIT_CRITICAL();

        i++;
        task_suspend_ticks(500);
    }

    // Sender finished its test burst; suspend indefinitely
    while (1) {
        task_suspend_ticks(1000);
    }
}

void task_receiver(void *args) {
    (void)args;
    uint8_t expected = 1;
    uint8_t all_received_ok = 1;

    while (expected <= RTOS_TEST_ITEMS) {
        suspend_until_queue_receive(tq_handle);

        void *res = queue_pop(tq_handle);
        uint8_t qpop = *(tq_handle->popped_item_buffer);

        if (res == NULL || qpop != expected) {
            all_received_ok = 0;
        }
        expected++;
    }

    TEST_ASSERT(all_received_ok == 1,
                "T4.Final: task_receiver woke up and popped all expected items in order");

    // Print final test summary once RTOS test completes
    tests_print_str("=== Queue Tests Summary: ");
    tests_print_dec(tests_run - tests_failed);
    tests_print_str("/");
    tests_print_dec(tests_run);
    tests_print_str(" passed ===\n");

    while (1) {
        task_suspend_ticks(1000);
    }
}

void tests_queue_runall(void) {
    TEST_INIT();

    tests_print_str("\n=== Starting Queue Tests ===\n");

    // 1. Run synchronous queue logic tests
    test_queue_single_send_pop();
    test_queue_fifo_order();

    // 2. Re-initialize queue and start RTOS task blocking tests
    reset_test_queue();

    task_sender_handle = task_create(0, coreMINIMUM_STACK_SIZE, task_sender, NULL);

    task_receiver_handle = task_create(0, coreMINIMUM_STACK_SIZE, task_receiver, NULL);

    os_start();
}
