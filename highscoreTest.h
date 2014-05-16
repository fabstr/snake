#ifndef HIGHSCORE_TEST_H
#define HIGHSCORE_TEST_H

#include <stdio.h>
#include <stdbool.h>
#include "highscore.hpp"
#include "testing.h"


void testMetadata(bool showPassedTests);
void testRecord(bool showPassedTests);
void testRecordWithSpaces(bool showPassedTests);
bool recordEqual(Record *r1, Record *r2);
void testHighscoreTable(bool showPassedTests);
void testAddRecord(bool showPassedTests);
void testInsertRecord(bool showPassedTests);

#endif
