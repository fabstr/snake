#include "stack.h"

int push(Stack *s, void *e)
{
	if (s->currPos >= s->size - 1) {
		/* we need to expand the stack */
		if (expand(s) != 0) {
			return 1;
		}
	}

	/* add to the stack */
	s->elements[++s->currPos] = e;

	return 0;
}

void* pop(Stack *s)
{
	void *toReturn = top(s);
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

	s->size =INITIALSTACKSIZE;
	s->currPos = -1;

	return s;
}

void freeStack(Stack *s)
{
	free(s->elements);
	free(s);
}
