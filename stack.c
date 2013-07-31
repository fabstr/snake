#include "stack.h"

int push(Stack *s, void *e)
{
	/* check if we need to expand the stack */
	if (s->currPos == s->size - 1) {
		if (expand(s) != 0) {
			return 1;
		}
	}

	/* move to the next index */
	s->currPos++;

	/* add the element */
	s->elements[s->currPos] = e;

	return 0;
}

void* pop(Stack *s)
{
	/* get the element */
	void *toReturn = top(s);

	/* update currPos */
	s->currPos--;

	return toReturn;
}

void* top(Stack *s)
{
	return s->elements[s->currPos];
}

int expand(Stack *s)
{
	/* double the size */
	size_t newSize = s->size + s->size;

	/* realloc */
	s->elements = realloc(s->elements, newSize*sizeof(void **));
	if (s->elements == NULL) {
		return 1;
	}

	/* update the size */
	s->size = newSize;

	return 0;
}

Stack* newStack()
{
	Stack *s = (Stack *) malloc(sizeof(Stack));
	if (s == NULL) {
		return NULL;
	}

	s->elements = (void **) malloc(sizeof(void *)*INITIALSTACKSIZE);
	if (s->elements == NULL) {
		return NULL;
	}

	s->size = INITIALSTACKSIZE;
	s->currPos = -1;

	return s;
}

void freeStack(Stack *s)
{
	free(s->elements);
	free(s);
}

void loopStack(Stack *s, void (*f)(void *))
{
	/* loop over the elements */
	for (int i=0; i<s->currPos+1; i++) {
		/* call the function */
		f(s->elements[i]);
	}
}

void* get(Stack *s, int i)
{
	if (i > s->currPos+1 || i < 0) {
		return NULL;
	} else {
		return s->elements[i];
	}
}
