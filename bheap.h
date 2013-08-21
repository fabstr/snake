#ifndef BHEAP_H
#define BHEAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mlog.h"

enum Modes {MIN, MAX};
/**< @brief min/max-binary heap */

typedef struct {
	void **heap;
	/**< @brief The heap array */
	
	size_t size;
	/**< @brief The size of the heap array */

	size_t nelements;
	/**< @brief The current number of elements in the heap */

	int (*compar)(void*, void*);
	/**< @brief A function to compare two elements. 
	 * Return negative if The first element is smaller than the second,
	 * positive if the first is larger and 0 if the elements are equal.
	 */

	enum Modes mode;
	/**< @brief Whether the heap is a min-heap or a max-heap. */
} BHeap;

/**
 * Create a new binary heap of the given size.
 * It should be free()-ed after use.
 * @param size The size of the heap.
 * @param compar A function to compare two elements.
 *		 Return negative if The first element is smaller than the 
 *		 second, positive if the first is larger and 0 if the elements
 *		 are equal.
 * @param mode Whether the heap is a min-heap or a max-heap.
 * @return The heap, or NULL on failure.
 */
BHeap *newBHeap(size_t size, int (*compar)(void*, void*), enum Modes mode);

/**
 * Add an element.
 * @param bheap The heap to add the element to.
 * @param element The element to add
 * @return 0 on success or -1 if the heap is full.
 */
int addElement(BHeap *bheap, void *element);

/**
 * Remove and return the topmost element.
 * @param bheap The heap to operate on
 * @return The topmost element, or NULL if the heap is empty.
 */
void *removeTop(BHeap *bheap);

/**
 * Return the topmost element.
 * @param bheap The heap to operate on
 * @return The topmost element, or NULL if the heap is empty.
 */
void *top(BHeap *bheap);

/**
 * Get the parent element.
 * @param index The index to get the parent of
 * @return The parent
 */
int getParent(int index);

/**
 * Swap the elements at i1 and i2.
 * @param i1 The first index
 * @param i2 The second index
 */
void swap(BHeap *bheap, int i1, int i2);

/**
 * Get the first child of parent.
 * @param index The parent
 * @return The first child
 */
int getFirstChild(int parent);

/**
 * Get the second child of parent.
 * @param index The parent
 * @return The second child
 */
int getSecondChild(int parent);

/**
 * Free the binary heap
 * @param bheap The binary heap to free
 */
void freeBHeap(BHeap *bheap);

/**
 * Print the binary heap.
 * @param bheap The heap to print
 * @param printFunc A function to print an element
 */
void printBHeap(BHeap *bheap, void (*printFunc)(void *));

/**
 * Helping function of printBHeap.
 * @param bheap The heap to print
 * @param printFunc A function to print an element
 * @param index The current node
 */
void printBHeapAux(BHeap *bheap, void (*printFunc)(void *), int index);

/**
 * Return true if the binary heap is healthy.
 * It is healthy is all the children of an element is smaller/larger than the
 * parent, depending on the mode (MIN/MAX).
 * @param bh The heap
 * @return True if the heap is healthy, else false.
 */
bool isHealthy(BHeap *bh);

/**
 * Helping function of isHealthy.
 * @param bh The heap to check 
 * @param index The current parent
 * @return True if the tree, starting on index, is healthy.
 */
bool isHealthyAux(BHeap *bh, int index);

/**
 * Return true if the element exists in the binary heap.
 * The element exist if the pointer element can be found in the binary heap.
 * @param element The element to check for existance.
 * @return True if the element exists, else false.
 */
bool exists(BHeap *bh, void *element);

/**
 * Helping function of exists.
 * @param element The element to check for existance.
 * @param index The current index (the current head of the tree)
 * @return True if the element exists, else false.
 */
bool existsAux(BHeap *bh, void *element, int index);
#endif
