#include <stdio.h>
#include <stdbool.h>
#include "highscore.h"

#ifndef HIGHSCORE_TEST_H
#define HIGHSCORE_TEST_H

static bool ShowPass = false;

void testMetadata();
void printRecord(Record *r);
void testRecord();
void testRecordWithSpaces();
bool recordEqual(Record *r1, Record *r2);
void testHighscoreTable();
void testAddRecord();
void testInsertRecord();

#endif
