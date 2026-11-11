#include "heap.h"

typedef uint16_t hblock_t;

#define H_IS_LAST_F     (1u << 0)   // flag saying if the hblock is last on the heap
#define H_IS_USED_F     (1u << 1)   // if hblock is used (set by hfree())
#define H_IS_PREV_USED  (1u << 2)   // if previous hblock is used 
#define H_SIZE_SHIFT    3           
#define H_SIZE_MASK     0xFFF8      //0b1111111111111000 // rest of the bitfield is occupied by hblock size

#define H_GET_SIZE(hblock) \
    ((hblock & H_SIZE_MASK) >> H_SIZE_SHIFT)

#define H_SET_SIZE(hblock, size) \
    (hblock |= (size << H_SIZE_SHIFT))


void *hmalloc(size_t size) {

}

void hfree(void *mem) {

}
