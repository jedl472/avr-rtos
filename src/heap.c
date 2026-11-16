#include "heap.h"
#include "core.h"
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

FORCE_INLINE uint8_t H_GET_FLAG(hblock_t *hblock, hblock_t flag_mask) {
    return ((*hblock & flag_mask) != 0);
}

FORCE_INLINE hblock_t *GET_HBLOCK(void *mem) {
    return (hblock_t *)((uint8_t *)mem - sizeof(hblock_t));
}

// pointer to the first hblock
extern char __heap_start;

// heap start specific to this module, pointer to the first hblock
char* h_heap_start = &__heap_start;

// pointer to the free address after the highest ever allocated on heap
uint8_t *h_brkval = NULL; 

// ================================================
// private functions
// ================================================

void combine_adjacement_hblocks(void) {
    if (h_brkval == NULL || h_brkval == (uint8_t *)(h_heap_start)) return;

    uint8_t *h = (uint8_t *)(h_heap_start);

    while (h < h_brkval) {
        hblock_t *curr = (hblock_t *)h;
        size_t curr_size = H_GET_SIZE(curr);
        uint8_t *next_h = h + sizeof(hblock_t) + curr_size;

        if (H_GET_FLAG(curr, H_IS_USED_FLAG) == 0 &&                // If current block is free
            next_h < h_brkval &&                                    // and there is a next block
            H_GET_FLAG((hblock_t *)next_h, H_IS_USED_FLAG) == 0) {  // and next block is also free
            
            size_t next_size = H_GET_SIZE((hblock_t *)next_h);
            H_SET_SIZE(curr, curr_size + sizeof(hblock_t) + next_size);
        } else {
            // pull brkval down if this is the topmost block
            if (next_h >= h_brkval && H_GET_FLAG(curr, H_IS_USED_FLAG) == 0) {
                h_brkval = h;
                break;
            }
            h = next_h;
        }
    }}

hblock_t *find_free_space(size_t size) {
    hblock_t *h = (hblock_t*)(h_heap_start);
    
    hblock_t *smallest_fitting_h = NULL;

    while ((uint8_t*)h < h_brkval) {
        if (H_GET_FLAG(h, H_IS_USED_FLAG) == 0 && H_GET_SIZE(h) >= size) {
            if(smallest_fitting_h == NULL || H_GET_SIZE(smallest_fitting_h) >= H_GET_SIZE(h)) {
                smallest_fitting_h = h;
            }
        }

        h = (hblock_t *)((uint8_t *)h + sizeof(hblock_t) + H_GET_SIZE(h));
    }
    
    if(smallest_fitting_h != NULL) {
        return smallest_fitting_h; 
    }
    
    // check if there is enough space at the end of heap
    size_t free_space = (uint8_t *)H_HEAP_END - h_brkval;
    if (free_space < sizeof(hblock_t) || size > free_space - sizeof(hblock_t)) {
        return NULL;
    } else {
        return (hblock_t*)h_brkval;
    }
}


// ================================================
// exposed functions
// ================================================

void *hmalloc(size_t size) {
    // set h_brkval to the start of the heap
    if(h_brkval == NULL) { // ~ means first allocation is being done, if h_brkval was not changed by user
        h_brkval = (uint8_t*)(h_heap_start);
    }

    combine_adjacement_hblocks();

    hblock_t *new_hblock = find_free_space(size);
    if (new_hblock == NULL) {
        return NULL;
    }

    // if allocating hblock at h_brkval
    if ((uint8_t *)new_hblock == h_brkval) {
        *new_hblock = 0;
        H_SET_SIZE(new_hblock, size);
        H_SET_FLAG_1(new_hblock, H_IS_USED_FLAG);
        h_brkval += sizeof(hblock_t) + size;
    } 
    // reusing an existing freed block
    else {
        size_t old_size = H_GET_SIZE(new_hblock);

        // make new hblock at the end of allocated space marking the free gap
        if (old_size >= size + sizeof(hblock_t) + 1) {
            hblock_t *split_block = (hblock_t *)((uint8_t *)new_hblock + sizeof(hblock_t) + size);
            *split_block = 0;
            H_SET_SIZE(split_block, old_size - size - sizeof(hblock_t));
            H_SET_FLAG_0(split_block, H_IS_USED_FLAG);

            H_SET_SIZE(new_hblock, size);
        }

        // or keep old size
        H_SET_FLAG_1(new_hblock, H_IS_USED_FLAG);
    }

    return (void *)((uint8_t *)new_hblock + sizeof(hblock_t));
}


// TODO: does not move brvkal, so is not even needed now
void hfree(void *mem) {
    if (mem == NULL) {
        return;
    }
    hblock_t *hblock = (hblock_t*)((uint8_t*)mem - sizeof(hblock_t));
    H_SET_FLAG_0(hblock, H_IS_USED_FLAG);

    combine_adjacement_hblocks();
}
