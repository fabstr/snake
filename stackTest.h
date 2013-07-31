#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"

#ifndef STACK_TEST_H
#define STACK_TEST_H

static bool ShowPass = false;

typedef struct testStruct {
	char *string;
} testStruct;
void printTestStruct(testStruct *ts);

void testNewStack();
void testPushAndPop();
void testPushPopMany();
void testTop();
void testExpand();

#endif
