/**
 * @file heap.h
 * @brief Heap memory management (DIY malloc and free right now).
 * 
 * Great source https://silent-tower.net/projects/visual-overview-malloc
 *
 * @todo set H_HEAP_END to something else than RAMEND
 */

#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

#include <avr/io.h>


typedef uint16_t hblock_t;

// WIP, TODO, and everything
#define H_HEAP_END RAMEND

// #define H_IS_LAST_FLAG      (1u << 0)   // flag saying if the hblock is last on the heap // flag not used nor set
#define H_IS_USED_FLAG      (1u << 1)   // if hblock is used (set by hfree())
// #define H_IS_PREV_USED_FLAG (1u << 2)   // if previous hblock is used  // this flag is not used nor set
#define H_SIZE_SHIFT        3           
#define H_SIZE_MASK         0xFFF8      //0b1111111111111000 
                                        // rest of the bitfield is occupied by the hblock size parameter

/**
 * @brief Allocates a chunk on the heap of size.
 * 
 * returns NULL, if allocation out of bounds of heap space
 */
void *hmalloc(size_t size);


/**
 * @brief Frees allocated memory.
 */
void hfree(void *mem);


#endif // HEAP_H
