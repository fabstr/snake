#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"
#include "testing.h"

#ifndef STACK_TEST_H
#define STACK_TEST_H

typedef struct testStruct {
	char *string;
} testStruct;
void printTestStruct(testStruct *ts);

void testNewStack(bool showPassedTests);
void testPushAndPop(bool showPassedTests);
void testPushPopMany(bool showPassedTests);
void testTop(bool showPassedTests);
void testExpand(bool showPassedTests);
void loopTestFunction(void *e);

#endif
