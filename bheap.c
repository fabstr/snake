#include "bheap.h"

BHeap *newBHeap(size_t size, int (*compar)(void*, void*), enum Modes mode) 
{
	BHeap *bh = (BHeap *) malloc(sizeof(BHeap));
	if (bh == NULL) {
		return NULL;
	}

	bh->heap = (void **) malloc(sizeof(void*) * size);
	if (bh->heap == NULL) {
		free(bh);
		return NULL;
	}

	bh->size = size;
	bh->nelements = 0;
	bh->compar = compar;
	bh->mode = mode;

	memset(bh->heap, '\0', sizeof(void *) * bh->size);

	return bh;
}

int addElement(BHeap *bheap, void *element)
{
	if (bheap->nelements == bheap->size) {
		return -1;
	}

	int current = bheap->nelements;
	int parent = getParent(current);
	bheap->heap[current] = element;

	int res;
	do {
		res = bheap->compar(bheap->heap[current ], 
				bheap->heap[parent]);
		if (bheap->mode == MIN) {
			/* swap negative/positive */
			res = -res;
		}

		if (res > 0) {
			swap(bheap, current , parent);
			current = parent;
			parent = getParent(current);
		} else {
			break;
		}
	} while (res > 0);

	bheap->nelements++;
	return 0;
}

void *removeTop(BHeap *bheap)
{
	if (bheap->nelements == 0) {
		return NULL;
	} else if (bheap->nelements == 1) {
		void *toReturn = top(bheap);
		bheap->nelements = 0;
		return toReturn;
	}

	int first = 0;
	int last = bheap->nelements-1;

	void *toReturn = bheap->heap[first];

	swap(bheap, first, last);

	int current = first;
	int res1 = 0;
	int res2 = 0;
	do {
		int child1 = getFirstChild(current);
		if (child1 < bheap->nelements) {
			res1 = bheap->compar(bheap->heap[current], 
					bheap->heap[child1]);
		}

		int child2 = getSecondChild(current);
		if (child2 < bheap->nelements) {
			res2 = bheap->compar(bheap->heap[current], 
					bheap->heap[child2]);
		}

		if (bheap->mode == MAX) {
			res1 = -res1;
			res2 = -res2;
		}

		if (res1 < 0) {
			swap(bheap, current, child1);
			current = child1;
			child1 = getFirstChild(current);
			child2 = getSecondChild(current);
		} else if (res2 < 0) {
			swap(bheap, current, child2);
			current = child2;
			child1 = getFirstChild(current);
			child2 = getSecondChild(current);
		} else {
			break;
		} 
	} while (res1 < 0 && res2 < 0);

	bheap->nelements--;

	return toReturn;
}

void *top(BHeap *bheap)
{
	if (bheap->nelements == 0) {
		return NULL;
	}

	return bheap->heap[0];
}

int getParent(int index)
{
	return index/2;
}

void swap(BHeap *bheap, int i1, int i2)
{
	void *tmp = bheap->heap[i1];
	bheap->heap[i1] = bheap->heap[i2];
	bheap->heap[i2] = tmp;
}

int getFirstChild(int parent)
{
	return parent*2+1;
}

int getSecondChild(int parent)
{
	return parent*2+2;
}

void freeBHeap(BHeap *bheap)
{
	free(bheap->heap);
	free(bheap);
}

void printBHeap(BHeap *bheap, void (*printFunc)(void *))
{
	if (bheap->nelements == 0) {
		return;
	}

	printf("Heap size: %ld\n", bheap->size);
	printf("Number of elements: %ld\n", bheap->nelements);
	printf("Mode: %s-heap\n", (bheap->mode == MIN) ? "min" : "max");
	printBHeapAux(bheap, printFunc, 0);
}

void printBHeapAux(BHeap *bheap, void (*printFunc)(void *), int index)
{
	if (index == bheap->nelements - 1) {
		return;
	}

	printf("Index %d: ", index);
	printFunc(bheap->heap[index]);

	int first = getFirstChild(index);
	if (first < bheap->nelements) {
		printf("\n Child 1, index %d: ", getFirstChild(index));
		printFunc(bheap->heap[getFirstChild(index)]);
	}

	int second = getSecondChild(index);
	if (second < bheap->nelements) {
		printf("\n Child 2, index %d: ", getSecondChild(index));
		printFunc(bheap->heap[getSecondChild(index)]);
	}

	printf("\n");

	if (first < bheap->nelements) {
		printBHeapAux(bheap, printFunc, getFirstChild(index));
	}

	if (second < bheap->nelements) {
		printBHeapAux(bheap, printFunc, getSecondChild(index));
	}
}

bool isHealthy(BHeap *bh)
{
	return isHealthyAux(bh, 0);
}

/* return true if both children of index are smaller/larger than index */
bool isHealthyAux(BHeap *bh, int index)
{
	if (index >= bh->nelements) {
		return true;
	}

	int res1 = 0;
	int res2 = 0;

	int first = getFirstChild(index);
	int second = getSecondChild(index);
	if (first < bh->nelements) {
		res1 = bh->compar(bh->heap[index], bh->heap[first]);
	}

	if (second < bh->nelements) {
		res2 = bh->compar(bh->heap[index], bh->heap[second]);
	}

	if (bh->mode == MAX) {
		res1 = -res1;
		res2 = -res2;
	}

	if (res1 <= 0 && res2 <= 0) {
		return isHealthyAux(bh, first) && isHealthyAux(bh, second);
	} else {
		return false;
	}
}

bool exists(BHeap *bh, void *element)
{
	return existsAux(bh, element, 0);
}

bool existsAux(BHeap *bh, void *element, int index)
{
	if (index >= bh->nelements) {
		return false;
	}

	return element == bh->heap[index] ||
		existsAux(bh, element, getFirstChild(index)) == true ||
		existsAux(bh, element, getSecondChild(index)) == true;
}
