#include "heap.h"
#include <stdint.h>




// gemini says, that this forces gcc to not ignore inline at -O0
#define FORCE_INLINE static inline __attribute__((always_inline))


FORCE_INLINE size_t H_GET_SIZE(hblock_t *hblock) {
    return ((*hblock & H_SIZE_MASK) >> H_SIZE_SHIFT);
}

FORCE_INLINE void H_SET_SIZE(hblock_t *hblock, size_t size) {
    *hblock = (*hblock & ~H_SIZE_MASK) |
              (size << H_SIZE_SHIFT);
}

FORCE_INLINE void H_SET_FLAG_0(hblock_t *hblock, hblock_t flag_mask) {
    *hblock = (*hblock & ~flag_mask);
}

FORCE_INLINE void H_SET_FLAG_1(hblock_t *hblock, hblock_t flag_mask) {
    *hblock = *hblock | flag_mask;
}

// pointer to the first hblock
extern char __heap_start;

// pointer to the free address after the highest ever allocated on heap
uint8_t *h_brkval = NULL; 


void *hmalloc(size_t size) {
    // set h_brkval to the start of the heap
    if(h_brkval == NULL) { // ~ means first allocation is being done, if h_brkval was not changed by user
        h_brkval = (uint8_t*)(&__heap_start);
    } 
    
    // check allocation bounds
    size_t free_space = (uint8_t *)H_HEAP_END - h_brkval;
    if (free_space < sizeof(hblock_t) || size > free_space - sizeof(hblock_t)) {
        return NULL;
    }
    
    // commented because H_IS_LAST_FLAG not used nor set
    // // if h_brkval is not at the start of the heap, clear H_IS_LAST_FLAG of preceeding hblock
    // if (h_brkval != (uint8_t*)(&__heap_start)) {
    //     H_SET_FLAG_0((hblock_t*)((uint8_t*)(h_brkval) - sizeof(hblock_t)), H_IS_LAST_FLAG);
    // }

    // find space for new hblock to allocate
    // TODO: add some more advanced way of finding free space to allocate from
    hblock_t *new_hblock = (hblock_t*)h_brkval;

    *new_hblock = 0;

    H_SET_SIZE(new_hblock, size);
    H_SET_FLAG_1(new_hblock, H_IS_USED_FLAG);
    
    h_brkval += sizeof(hblock_t) + size;

    return (void*)((uint8_t*)new_hblock + sizeof(hblock_t));
}

// TODO: does not move brvkal, so is not even needed now
void hfree(void *mem) {
    if (mem == NULL) {
        return;
    }
    hblock_t *hblock = (hblock_t*)((uint8_t*)mem - sizeof(hblock_t));
    H_SET_FLAG_0(hblock, H_IS_USED_FLAG);
}
