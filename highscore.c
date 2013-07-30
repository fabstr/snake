#include "highscore.h"

int readMetadata(char *line, HighscoreMetadata *metadata) 
{
	int count; 
	if (sscanf(line, "Count=%d\n", &count) == 0) {
		return 1;
	}

	metadata->count = count;

	return 0;
}

HighscoreTable *loadHighscoreFromFile(char *file) 
{
	/* open the highscore file */
	FILE *f = fopen(file, "rb");

	/* to hold a line of the file */
	char *line = NULL;

	/* read the first line */
	size_t linecap = 0;
	getline(&line, &linecap, f);

	/* first read the metadata (the number of records) */
	HighscoreMetadata metadata;
	readMetadata(line, &metadata);

	/* the table to place the records in */
	HighscoreTable *ht = (HighscoreTable *) malloc(sizeof(HighscoreTable));

	/* make the entries array fit the number of records */
	ht->records = (Record *) malloc(metadata.count*sizeof(Record));

	/* set the size of the array */
	ht->count = metadata.count;

	/* read the lines and parse them */
	int p = 0; /* the index of the entries array */
	while (getline(&line, &linecap, f) > 0) {
		/* get the current position in the array */
		Record *currRecord = &ht->records[p++];
		
		/* parse the current entry into the array */
		if (parseLine(line, currRecord) != 0) {
			freeHighscoreTable(ht);
			return NULL;
		}
	}

	ht->p = p;
	ht->count = metadata.count;

	free(line);

	fclose(f);

	return ht;
}

int parseLine(char *line, Record *dest)
{
	/* allocate memory for the name */
	dest->playerName = (char *) malloc(64);

	if (sscanf(line, "Score=%d Time=%ld Name=%64[0-9A-Za-z ]s\n", 
				&dest->score, &dest->timestamp, 
				dest->playerName) <= 0) {
		return 1;
	}

	return 0;
}

int writeMetadata(FILE *f, HighscoreTable *ht)
{
	if (fprintf(f, "Count=%d\n", ht->count) > 0) {
		return 0;

	} else {
		return 1;
	}
}

char *getRecordLine(Record *r)
{
	char *line;
	if (asprintf(&line, "Score=%d Time=%ld Name=%s", r->score, 
				r->timestamp, r->playerName) == -1) {
		return NULL;
	}

	return line;
}

int writeRecord(FILE *f, Record *r)
{
	char *line = getRecordLine(r);
	if (fprintf(f, "%s\n", line) < 0) {
		return 0;
	} else {
		free(line);
		return 1;
	}
}

int writeHighscoreToFile(char *file, HighscoreTable *ht)
{
	FILE *f = fopen(file, "w+b");

	if (writeMetadata(f, ht) != 0) {
		return 1;
	}

	/* the position in the array of records */
	int p = 0;

	/* write every entry to file */
	while (p < ht->p) {
		writeRecord(f, &ht->records[p]);
		p++;
	}

	fclose(f);
	return 0;
}

void freeHighscoreTable(HighscoreTable *ht) {
	for (int i=0; i<ht->count; i++) {
		free(ht->records[i].playerName);
	}
	free(ht->records);
	free(ht);
}

int addRecordToHighscoreTable(Record *r, HighscoreTable *ht) 
{
	if (ht->p >= ht->count) {
		/* we need to increase the size of the records array */
		size_t newSize = (ht->count+1)*sizeof(Record);

		if ((ht->records = realloc(ht->records, newSize)) == NULL) {
			return 1;
		}
	}

	/* add the record and update p/count */
	ht->records[ht->p] = *r;
	ht->p++;
	ht->count++;

	return 0;
}

int compareRecords(const void *e1, const void *e2)
{
	Record *r1 = (Record *) e1;
	Record *r2 = (Record *) e2;

	if (r1->score < r2->score) {
		return 1;
	} else if (r1->score == r2->score) {
		return 0;
	} else {
		return -1;
	}
}

int insertRecordAndSort(Record *r, HighscoreTable *ht)
{
	/* add the record */
	if (addRecordToHighscoreTable(r, ht) != 0) {
		return 1;
	}
	
	/* sort the records */
	qsort(ht->records, ht->p, sizeof(Record), compareRecords);

	return 0;
}
