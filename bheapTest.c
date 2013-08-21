#include "bheapTest.h" 
int compare(void *e1, void *e2)
{
	struct testStruct *ts1 = (struct testStruct *) e1;
	struct testStruct *ts2 = (struct testStruct *) e2;
	if (ts1->i < ts2->i) {
		return -1;
	} else if (ts1->i > ts2->i) {
		return 1;
	} else {
		return 0;
	}
}

void testNew(bool showPassedTests)
{
	BHeap *bh = newBHeap(10, compare, MIN);
	if (bh == NULL) {
		fmsg("testNew, could not allocate memory");
	} else if (bh->size != 10) {
		fmsg("testNew, size != 10, is %ld", bh->size);
	} else if (bh->nelements != 0) {
		fmsg("testNew, nelements != 0, is %ld", bh->nelements);
	} else if (bh->mode != MIN) {
		fmsg("testNew, mode != MIN");
	} else {
		pmsg("testNew");
	}

	freeBHeap(bh);
}

void testAdd(bool showPassedTests)
{
	struct testStruct toAdd = {.i = 3};
	BHeap *bh = newBHeap(1, compare, MIN);
	addElement(bh, &toAdd);
	struct testStruct *added = bh->heap[0];
	if (added->i != toAdd.i) {
		fmsg("testTop, added->i != toAdd->i, is %d and %d",  added->i, 
				toAdd.i);
	} else {
		pmsg("testTop");
	}

	freeBHeap(bh);
}

void testTop(bool showPassedTests)
{
	struct testStruct toAdd = {.i = 3};
	BHeap *bh = newBHeap(1, compare, MIN);
	addElement(bh, &toAdd);
	struct testStruct *added = top(bh);
	if (added->i != toAdd.i) {
		fmsg("testAdd, added->i != toAdd->i, is %d and %d",  added->i, 
				toAdd.i);
	} else if (bh->nelements != 1) {
		fmsg("testAdd, nelements not 1, is %ld", bh->nelements);
	} else {
		pmsg("testAdd");
	}

	freeBHeap(bh);
}

void testRemove(bool showPassedTests)
{
	struct testStruct toAdd1 = {.i = 3};
	struct testStruct toAdd2 = {.i = 4};
	BHeap *bh = newBHeap(2, compare, MIN);
	addElement(bh, &toAdd1);
	addElement(bh, &toAdd2);

	struct testStruct *removed = removeTop(bh);
	if (removed != &toAdd1) {
		fmsg("testRemove, removed = %p != %p, should be %p", &toAdd1,
				removed, &toAdd1);
	}

	struct testStruct *topElement = top(bh);
	if (topElement != &toAdd2) {
		fmsg("testRemove, top = %p != %p, should be %p", topElement,
				&toAdd2, &toAdd2);
	} else {
		pmsg("testRemove");
	}
	freeBHeap(bh);
}

void testRemoveReturnsNull(bool showPassedTests) 
{
	struct testStruct toAdd1 = {.i = 3};
	struct testStruct toAdd2 = {.i = 4};
	BHeap *bh = newBHeap(2, compare, MIN);
	addElement(bh, &toAdd1);
	addElement(bh, &toAdd2);

	removeTop(bh);
	removeTop(bh);
	struct testStruct *removed = removeTop(bh);
	if (removed != NULL) {
		fmsg("testRemoveReturnsNull, removed = %p != NULL", removed);
	} else {
		pmsg("testRemoveReturnsNull");
	}
	freeBHeap(bh);
}

void testParent(bool showPassedTests)
{
	int child1 = 5;
	int child2 = 4;
	if (getParent(child1) != 2) {
		fmsg("testParent, answer was not %d", child1/2);
	} else if (getParent(child2) != 2) {
		fmsg("testParent, answer was not %d", child2/2);
	} else {
		pmsg("testParent");
	}
}

void testFirstChild(bool showPassedTests)
{
	int i=0;
	if (getFirstChild(i) != i*2+1) {
		fmsg("testFirstChild, answer was not %d", i*2+1);
	} else {
		pmsg("testFirstChild");
	}
}

void testSecondChild(bool showPassedTests)
{
	int i=0;
	if (getSecondChild(i) != i*2+2) {
		fmsg("testFirstChild, answer was not %d", i*2+2);
	} else {
		pmsg("testFirstChild");
	}
}

void printTestStruct(void *e)
{
	if (e == NULL) {
		return;
	}

	struct testStruct *ts = (struct testStruct *) e;
	printf("{.i=%d}", ts->i);
}

void testRemoveMany(bool showPassedTests) 
{
	struct testStruct ts1 = {.i=9};
	struct testStruct ts2 = {.i=8};
	struct testStruct ts3 = {.i=7};
	struct testStruct ts4 = {.i=6};
	
	BHeap *bh = newBHeap(4, compare, MIN);

	addElement(bh, &ts1);
	addElement(bh, &ts2);
	addElement(bh, &ts3);
	addElement(bh, &ts4);

	removeTop(bh); /* removing ts4 */
	removeTop(bh); /* removing ts3 */
	removeTop(bh); /* removing ts2 */

	if (bh->nelements != 1) {
		fmsg("testRemoveMany, nelements = %ld, should be 1", 
				bh->nelements);
	} else if (isHealthy(bh) == false) {
		fmsg("testRemoveMany, not healthy");
	} else if (top(bh) != &ts1) {
		fmsg("testRemoveMany, top is not ts1 (%p), is %p", &ts1,
				top(bh));
	} else {
		pmsg("testRemoveMany");
	}

	freeBHeap(bh);
}

void testAddMany(bool showPassedTests)
{
	struct testStruct ts1 = {.i=9};
	struct testStruct ts2 = {.i=8};
	struct testStruct ts3 = {.i=7};
	struct testStruct ts4 = {.i=6};
	
	BHeap *bh = newBHeap(4, compare, MIN);

	addElement(bh, &ts1);
	addElement(bh, &ts2);
	addElement(bh, &ts3);
	addElement(bh, &ts4);
	if (isHealthy(bh) == false) {
		fmsg("testAddMany, not healthy");
	} else {
		struct testStruct *first = top(bh);
		if (first->i != 6) {
			fmsg("testAddMany, first != 6, is %d", first->i);
		} else {
			pmsg("testAddMany");
		}
	}

	freeBHeap(bh);
}

void testExists(bool showPassedTests)
{
	struct testStruct ts1 = {.i=9};
	struct testStruct ts2 = {.i=8};
	struct testStruct ts3 = {.i=7};
	struct testStruct ts4 = {.i=6};
	
	BHeap *bh = newBHeap(4, compare, MIN);

	struct testStruct *toFind = &ts1;

	addElement(bh, &ts1);
	addElement(bh, &ts2);
	addElement(bh, &ts3);
	addElement(bh, &ts4);
	if (isHealthy(bh) == false) {
		fmsg("testExists, not healthy");
	} else {
		if (exists(bh, toFind) == false) {
			fmsg("testExists, could not find the element");
		} else {
			pmsg("testExists");
		}
	}

	freeBHeap(bh);
	
}

void testExistsNot(bool showPassedTests)
{
	struct testStruct ts1 = {.i=9};
	struct testStruct ts2 = {.i=8};
	struct testStruct ts3 = {.i=7};
	struct testStruct ts4 = {.i=6};
	
	BHeap *bh = newBHeap(4, compare, MIN);

	struct testStruct *toFind = NULL;

	addElement(bh, &ts1);
	addElement(bh, &ts2);
	addElement(bh, &ts3);
	addElement(bh, &ts4);
	if (isHealthy(bh) == false) {
		fmsg("testExists, not healthy");
	} else {
		if (exists(bh, toFind) == true) {
			fmsg("testExists, could find the element");
		} else {
			pmsg("testExists");
		}
	}

	freeBHeap(bh);
	
}

int main(int argc, char **argv)
{

	bool showPassedTests = false;
	if (argc > 1) {
		if (strcmp("--show-passed=yes", argv[1]) == 0) {
			showPassedTests = true;
		} else {
			showPassedTests = false;
		}
	}

	testNew(showPassedTests);
	testAdd(showPassedTests);
	testTop(showPassedTests);
	testRemove(showPassedTests);
	testParent(showPassedTests);
	testFirstChild(showPassedTests);
	testSecondChild(showPassedTests);
	testAddMany(showPassedTests);
	testRemoveMany(showPassedTests);
	testExists(showPassedTests);
	testExistsNot(showPassedTests);
	testRemoveReturnsNull(showPassedTests);

	return 0;
}
