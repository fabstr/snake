#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

typedef struct Record {
	int score;
	time_t timestamp;
	char *playerName;
} Record;

typedef struct HighscoreTable {
	Record *records; /* the array of the records */
	int count; /* the total number of records */
	int p; /* the latest non-occupied position of the array */
} HighscoreTable;

typedef struct HighscoreMetadata {
	int count;
} HighscoreMetadata;

int readMetadata(char *line, HighscoreMetadata *metadata);
HighscoreTable* loadHighscoreFromFile(char *file);
int parseLine(char *line, Record *dest);
int writeMetadata(FILE *f, HighscoreTable *ht);
int writeRecord(FILE *f, Record *r);
int writeHighscoreToFile(char *file, HighscoreTable *ht);
void freeHighscoreTable(HighscoreTable *ht);
int addRecordToHighscoreTable(Record *r, HighscoreTable *ht);
int insertRecordAndSort(Record *r, HighscoreTable *ht);

/**
 * Compare the Records e1 and e2.
 * If e1's score < e2, return 1.
 * If e1's score = e2, return 0.
 * If e1's score > e2, return -1.
 */
int compareRecords(const void *e1, const void *e2);


#endif
