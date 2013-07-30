#include "stackTest.h"

void testNewStack()
{
	Stack *s = newStack();
	
	if (s->size != 10) {
		printf("TEST FAILURE: testNewStack, size not 10\n");
	} else if (s->currPos != -1) {
		printf("TEST FAILURE: testNewStack, currPos not -1\n");
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
	} else if (s->currPos != 0) {
		printf("TEST FAILURE: testTop, currPos != 0, is %d\n", 
				s->currPos);
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
		printf("TEST FAILURE: testPushAndPop, currPos != -1, is %d\n", 
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
		printf("TEST FAILURE: testPushPopMany, array failure\n");
	} else {
		if (ShowPass == true) {
			printf("PASS: testPushPopMany\n");
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

	return 0;
}
