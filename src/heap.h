/**
 * @file heap.h
 * @brief Heap memory management (DIY malloc and free right now).
 * 
 * Great source https://silent-tower.net/projects/visual-overview-malloc
 *
 * @todo is very dumb right now, does not merge adjacement free blocks. WIP
 * @todo set H_HEAP_END to something else than RAMEND
 */

#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

#include <avr/io.h>

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
