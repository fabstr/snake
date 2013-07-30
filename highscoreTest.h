#include <stdio.h>
#include <stdbool.h>
#include "highscore.h"

#ifndef HIGHSCORE_TEST_H
#define HIGHSCORE_TEST_H

void testMetadata();
void printRecord(Record *r);
void testRecord();
bool recordEqual(Record *r1, Record *r2);
void testHighscoreTable();
void testAddRecord();
void testInsertRecord();

#endif
