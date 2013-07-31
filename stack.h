#include <stdlib.h>
#include "mlog.h"
#include <stdio.h>

#ifndef STACK_H
#define STACK_H

#define INITIALSTACKSIZE 10

typedef struct Stack {
	/* the current size of the elements array */
	size_t size;

	/* -1 if the stack is empty */
	int currPos;

	/* the data of the stack */
	void **elements;
} Stack;

/**
 * Push e onto the stack.
 * @param s The stack
 * @param e The element to push.
 * @return 0 on success, 1 if the stack could not be expanded.
 */
int push(Stack *s, void *e);

/**
 * Remove and return the topmost element of the stack.
 * @param s The stack
 * @return The topmost element
 */
void* pop(Stack *s);

/**
 * Return the topmost element of the stack.
 * @param s The stack
 * @return The topmost element
 */
void* top(Stack *s);

/**
 * Expand the stack, making it twice as big.
 * @param s The stack to expand
 * @return 0 on success, else 1.
 */
int expand(Stack *s);

/**
 * Create a new stack.
 * The size will be 10.
 * @return The new stack, or NULL on memory allocation failure.
 */
Stack* newStack();

/**
 * Free the stack.
 * The CONTENTS of the stack is not free'd.
 * @param s The stack to free
 */
void freeStack(Stack *s);

/**
 * Loop over the elements in the stack, treating it as an array.
 * @param s The stack
 * @param f The function to call for each element. It should accept a void* as 
 * 	    argument and return void.
 */
void loopStack(Stack *s, void (*f)(void *));

/**
 * Get the element at the position.
 * @param s The stack
 * @param i The index to get
 * @return The element, or NULL if it don't exist.
 */
void* get(Stack *s, int i);

#endif
