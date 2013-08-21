#ifndef BHEAPTEST_H
#define BHEAPTEST_H

#include <stdio.h>
#include <string.h>
#include "bheap.h"
#include "testing.h"

struct testStruct {
	int i;
};

int compare(void *e1, void *e2);

void testNew(bool showPassedTests);
void testAdd(bool showPassedTests);
void testTop(bool showPassedTests);
void testRemove(bool showPassedTests);
void testParent(bool showPassedTests);
void testFirstChild(bool showPassedTests);
void testSecondChild(bool showPassedTests);
void testAddMany(bool showPassedTests);
void testRemoveMany(bool showPassedTests);
void printTestStruct(void *e);
void testRemoveReturnsNull(bool showPassedTests);

#endif
