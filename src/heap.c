#include "heap.h"
#include <stdint.h>


typedef uint16_t hblock_t;


// gemini says, that this forces gcc to not ignore inline at -O0
#define FORCE_INLINE static inline __attribute__((always_inline))



#define HBLOCK_SIZE_BYTES sizeof(hblock_t)

#define H_IS_LAST_FLAG      (1u << 0)   // flag saying if the hblock is last on the heap
#define H_IS_USED_FLAG      (1u << 1)   // if hblock is used (set by hfree())
#define H_IS_PREV_USED_FLAG (1u << 2)   // if previous hblock is used 
#define H_SIZE_SHIFT        3           
#define H_SIZE_MASK         0xFFF8      //0b1111111111111000 
                                        // rest of the bitfield is occupied by the hblock size parameter

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

// highest address ever allocated on the heap
uint8_t *h_brkval = NULL; 


// !!!!!!!!!!!!!!!!!!!!! WIP !!!!!!!!!!!!!!!!!!!!!!!!!!!
void *hmalloc(size_t size) {
    if(h_brkval == NULL) h_brkval = (uint8_t*)(&__heap_start);
    
    // TODO: add some more advanced way of finding free space to allocate from
    hblock_t *new_hblock = (hblock_t*)h_brkval;


    H_SET_SIZE(new_hblock, size);
    H_SET_FLAG_1(new_hblock, H_IS_LAST_FLAG);
    
}

void hfree(void *mem) {

}
