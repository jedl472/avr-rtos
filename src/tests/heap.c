#include "heap.h"
#include "tests.h"
#include <stdint.h>

#define TEST_HEAP_SIZE 128

// Access h_brkval from heap.c so we can sandbox and reset the heap between tests
extern uint8_t *h_brkval;

extern char __heap_start;
extern char* h_heap_start;

// Mirror the internal block definitions from heap.c to inspect headers in tests

static inline size_t test_get_size(hblock_t *hblock) {
    return ((*hblock & H_SIZE_MASK) >> H_SIZE_SHIFT);
}

static inline uint8_t test_get_flag(hblock_t *hblock, hblock_t flag_mask) {
    return ((*hblock & flag_mask) != 0);
}

static inline hblock_t *get_header(void *mem) {
    return (hblock_t *)((uint8_t *)mem - sizeof(hblock_t));
}

// 128-byte sandboxed buffer so tests don't touch the real heap
#define TEST_HEAP_SIZE 128
static uint8_t test_heap_buffer[TEST_HEAP_SIZE];

static void reset_test_heap(void) {
    memset(test_heap_buffer, 0xAA, sizeof(test_heap_buffer));
    h_heap_start = (char *)&test_heap_buffer[0];
    // Point h_brkval at our sandbox buffer so hmalloc allocates inside it
    h_brkval = &test_heap_buffer[0];
}

// Test 1: single allocation and bitfield header encoding
void test_single_allocation(void) {
    reset_test_heap();

    void *p1 = hmalloc(16);
    TEST_ASSERT(p1 != NULL, "T1.1: hmalloc(16) returns non-NULL");
    TEST_ASSERT((uint8_t *)p1 == &test_heap_buffer[sizeof(hblock_t)],
                "T1.2: Payload starts sizeof(hblock_t) after block start");

    hblock_t *hdr1 = get_header(p1);
    TEST_ASSERT(test_get_size(hdr1) == 16, "T1.3: Size 16 valid in hblock.");
    TEST_ASSERT(test_get_flag(hdr1, H_IS_USED_FLAG), "T1.4: H_IS_USED_FLAG is set");
    TEST_ASSERT(h_brkval == (uint8_t *)p1 + 16, "T1.5: h_brkval advanced by header + 16");
}

// Test 2: multiple allocations and header boundary integrity
void test_multiple_allocations(void) {
    reset_test_heap();

    uint8_t *p1 = (uint8_t *)hmalloc(10);
    memset(p1, 0x55, 10);

    uint8_t *p2 = (uint8_t *)hmalloc(20);
    memset(p2, 0x33, 20);

    hblock_t *hdr1 = get_header(p1);
    hblock_t *hdr2 = get_header(p2);

    TEST_ASSERT(p2 == p1 + 10 + sizeof(hblock_t),
                "T2.1: p2 starts immediately after p1 payload + hdr2");

    uint8_t p1_intact = 1;
    for (uint8_t i = 0; i < 10; i++) {
        if (p1[i] != 0x55) p1_intact = 0;
    }
    TEST_ASSERT(p1_intact == 1, "T2.2: Allocating p2 did not corrupt p1 payload");
    TEST_ASSERT(test_get_size(hdr1) == 10, "T2.3: hdr1 size is still 10");
    TEST_ASSERT(test_get_size(hdr2) == 20, "T2.4: hdr2 size is 20");
    TEST_ASSERT(test_get_flag(hdr2, H_IS_USED_FLAG), "T2.5: hdr2 H_IS_USED_FLAG is set");
}

// Test 3: bounds check against H_HEAP_END (RAMEND)
void test_bounds_and_oom(void) {
    reset_test_heap();

    // Calculate total distance from our sandbox to RAMEND
    size_t space_to_ramend = (uint8_t *)RAMEND - h_brkval;

    // Request 1 byte more than available space
    void *p_oom = hmalloc(space_to_ramend);
    TEST_ASSERT(p_oom == NULL, "T3.1: Allocation exceeding RAMEND returns NULL");

    // Request 0xFFFF to test 16-bit integer overflow protection
    void *p_ovf = hmalloc(0xFFFF);
    TEST_ASSERT(p_ovf == NULL, "T3.2: Huge size (0xFFFF) returns NULL safely");

    // Ensure h_brkval was not modified by failed allocations
    TEST_ASSERT(h_brkval == &test_heap_buffer[0], "T3.3: Failed hmalloc leaves h_brkval unchanged");
}

// Test 4: hfree() flag clearing and NULL safety
void test_hfree(void) {
    reset_test_heap();

    void *p1 = hmalloc(12);
    void *p2 = hmalloc(16);

    hblock_t *hdr1 = get_header(p1);
    hblock_t *hdr2 = get_header(p2);

    hfree(p1);
    TEST_ASSERT(!test_get_flag(hdr1, H_IS_USED_FLAG), "T4.1: hfree(p1) clears H_IS_USED_FLAG");
    TEST_ASSERT(test_get_size(hdr1) == 12, "T4.2: hfree(p1) preserves block size (12)");
    TEST_ASSERT(test_get_flag(hdr2, H_IS_USED_FLAG), "T4.3: hfree(p1) does not affect p2");

    hfree(NULL);
    TEST_ASSERT(1, "T4.4: hfree(NULL) is a safe no-op");
}

// Test 5: optimizing heap space
void test_heap_space_optimizing(void) {
    reset_test_heap();

    void *p1 = hmalloc(12);
    void *p2 = hmalloc(16);
    void *p4 = hmalloc(4); // so brkval does not get moved down
    hfree(p1);
    hfree(p2);

    hblock_t *hdr1 = get_header(p1);

    TEST_ASSERT(test_get_size(hdr1) == (12 + 16 + sizeof(hblock_t)), "T5.1: adjacement hblocks get combined correctly");

    void *p3 = hmalloc(8);

    hblock_t *hdr3 = get_header(p3);

    TEST_ASSERT((char*)hdr3 == h_heap_start, "T5.2: hmalloc allocates mem in the free space");

    hblock_t *p3_leftover = (hblock_t *)((uint8_t *)p3 + test_get_size(hdr3));

    TEST_ASSERT(test_get_size(p3_leftover) == 20, "T5.3: leftover hblocks after hmalloc have correct size");
    
    hfree(p4);
    TEST_ASSERT(h_brkval == (uint8_t *)p3_leftover, "T5.4: hfree moves h_brkval down across adjacent free blocks");

    // Free remaining block p3: should pull h_brkval all the way back to h_heap_start
    hfree(p3);
    TEST_ASSERT(h_brkval == (uint8_t *)h_heap_start, "T5.5: hfree resets h_brkval to h_heap_start when heap is empty");
}


void tests_heap_runall(void) {
    TEST_INIT();
    
    char *saved_heap_start = h_heap_start;
    uint8_t *saved_brkval = h_brkval; // Save real heap state

    tests_print_str("=== Starting Heap Tests ===\n");
    test_single_allocation();
    test_multiple_allocations();
    test_bounds_and_oom();
    test_hfree();
    test_heap_space_optimizing();

    tests_print_str("=== Summary: ");
    tests_print_dec(tests_run - tests_failed);
    tests_print_str("/");
    tests_print_dec(tests_run);
    tests_print_str(" passed ===\n");

    h_brkval = saved_brkval; // Restore real heap state after testing
    h_heap_start = saved_heap_start;
}
