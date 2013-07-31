#include "stackTest.h"

void printTestStruct(testStruct *ts)
{
	printf("{.string='%s'}\n", ts->string);
}

void testNewStack()
{
	Stack *s = newStack();
	
	if (s->size != 10) {
		printf("TEST FAILURE: testNewStack, size not 10\n");
	} else if (s->currPos != -1) {
		printf("TEST FAILURE: testNewStack, currPos not NULL\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testNewStack\n");
		}
	}

	freeStack(s);
}

void testTop()
{
	Stack *s = newStack();

	int x = 42;
	int *y;
	
	push(s, &x);

	y = (int *) top(s);

	if (x != *y) {
		printf("TEST FAILURE: testTop, x = %d != %d\n", x, *y);
	} else {
		if (ShowPass == true) {
			printf("PASS: testTop\n");
		}
	}

	freeStack(s);
}

void testPushAndPop()
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
		printf("TEST FAILURE: testPushAndPop, x = %d != %d\n", x, *y);
	} else if (s->currPos != -1) {
		printf("TEST FAILURE: testPushAndPop, currPos != 0, is %d\n", 
				s->currPos);
	} else {
		if (ShowPass == true) {
			printf("PASS: testPushAndPop\n");
		}
	}

	freeStack(s);
}

void testPushPopMany() 
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
		printf("TEST FAILURE: testPushPopMany, array failure\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testPushPopMany\n");
		}
	}

	freeStack(s);
}

void testExpand()
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
		printf("TEST FAILURE: testExpand, array failure\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testExpand\n");
		}
	}

	freeStack(s);
}

void testInitialStackSize()
{
	if (INITIALSTACKSIZE != 10) {
		printf("TEST FAILURE: testInitialStackSize, not 10\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testInitialStackSize\n");
		}
	}
}

void loopTestFunction(void *e)
{
	testStruct *lts = (testStruct *) e;
	lts->string = "touched";
}

void testLoopStack()
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
		printf("TEST FAILURE: testLoopStack\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testLoopStack\n");
		}
	}

	freeStack(s);
}

int main(int argc, char **argv)
{
	if (argc > 1) {
		if (strcmp("--show-passed=yes", argv[1]) == 0) {
			ShowPass = true;
		} else {
			ShowPass = false;
		}
	}

	testNewStack();
	testTop();
	testPushAndPop();
	testPushPopMany();
	testExpand();
	testInitialStackSize();
	testLoopStack();

	return 0;
}
