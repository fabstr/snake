#include "stackTest.h"

void printTestStruct(testStruct *ts)
{
	printf("{.string='%s'}", ts->string);
}

void testNewStack(bool showPassedTests)
{
	Stack *s = newStack();

	if (s->size != 10) {
		fmsg("testNewStack, size not 10");
	} else if (s->currPos != -1) {
		fmsg("testNewStack, currPos not NULL");
	} else {
		pmsg("testNewStack");
	}

	freeStack(s);
}

void testTop(bool showPassedTests)
{
	Stack *s = newStack();

	int x = 42;
	int *y;

	push(s, &x);

	y = (int *) top(s);

	if (x != *y) {
		fmsg("testTop, x = %d != %d", x, *y);
	} else {
		pmsg("testTop");
	}

	freeStack(s);
}

void testPushAndPop(bool showPassedTests)
{
	/* create the stack and an integer */
	Stack *s = newStack();
	int x = 42;
	int *y;

	/* push the integers */
	push(s, &x);

	/* pop it into y */
	y = (int *) pop(s);

	if (x != *y) {
		fmsg("testPushAndPop, x = %d != %d", x, *y);
	} else if (s->currPos != -1) {
		fmsg("testPushAndPop, currPos != 0, is %d", 
				s->currPos);
	} else {
		pmsg("testPushAndPop");
	}

	freeStack(s);
}

void testPushPopMany(bool showPassedTests)
{
	Stack *s = newStack();
	int arr[7] = {0, 1, 2, 3, 4, 5, 6};
	for (int i=0; i<7; i++) {
		push(s, &arr[i]);
	}

	bool failure = false;
	for (int i=0; i<7; i++) {
		int *y = (int *) pop(s);
		if (arr[6-i] != *y) {
			failure = true;
		}
	}

	if (failure == true) {
		fmsg("testPushPopMany, array failure");
	} else {
		pmsg("testPushPopMany");
	}

	freeStack(s);
}

void testExpand(bool showPassedTests)
{
	Stack *s = newStack();
	int arr[27];
	for (int i=0; i<27; i++) {
		arr[i] = i;
		push(s, &arr[i]);
	}

	bool failure = false;
	for (int i=0; i<27; i++) {
		int *y = (int *) pop(s);
		if (arr[26-i] != *y) {
			failure = true;
		}
	}

	if (failure == true) {
		fmsg("testExpand, array failure");
	} else {
		pmsg("testExpand");
	}

	freeStack(s);
}

void testInitialStackSize(bool showPassedTests)
{
	if (INITIALSTACKSIZE != 10) {
		fmsg("testInitialStackSize, not 10");
	} else {
		pmsg("testInitialStackSize");
	}
}

void loopTestFunction(void *e)
{
	testStruct *lts = (testStruct *) e;
	lts->string = "touched";
}

void testLoopStack(bool showPassedTests)
{
	Stack *s = newStack();
	testStruct arr[4] = {[0]={.string="zero"}, [1]={.string="one"},
		[2]={.string="two"}, [3]={.string="three"}};

	push(s, &arr[3]);
	push(s, &arr[2]);
	push(s, &arr[1]);
	push(s, &arr[0]);

	loopStack(s, loopTestFunction);

	bool passed = true;
	for (int i=0; i<4; i++) {
		testStruct *e1 = (testStruct *) s->elements[i];
		if (strcmp(e1->string, "touched") != 0) {
			passed = false;
		}
	}

	if (passed == false) {
		fmsg("testLoopStack");
	} else {
		pmsg("testLoopStack");
	}

	freeStack(s);
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

	testNewStack(showPassedTests);
	testTop(showPassedTests);
	testPushAndPop(showPassedTests);
	testPushPopMany(showPassedTests);
	testExpand(showPassedTests);
	testInitialStackSize(showPassedTests);
	testLoopStack(showPassedTests);

	return 0;
}
