#include "highscoreTest.h"

void testMetadata(bool showPassedTests)
{
	/* write to file */
	FILE *f = fopen("highscore-test.txt", "w+b");
	HighscoreTable ht;
	ht.count = 3;
	writeMetadata(f, &ht);
	fclose(f);

	/* read from file  */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse */
	HighscoreMetadata hm;
	readMetadata(line, &hm);
	free(line);

	/* check */
	if (hm.count != 3) {
		fmsg("testMetadata, count != 3, is %d", hm.count);
	} else {
		pmsg("testMetadata");
	}
}

void testRecord(bool showPassedTests)
{
	/* write the record */
	FILE *f = fopen("highscore-test.txt", "w+b");
	Record r = {.score=123, .timestamp=123456, .playerName="name"};
	writeRecord(f, &r);
	fclose(f);

	/* read the record */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse the record */
	Record r2;
	parseLine(line, &r2);

	if (r2.score != r.score || r2.timestamp != r.timestamp || 
			strcmp(r2.playerName, r.playerName) != 0) {
		fmsg("testRecord, records differ");
	} else {
		pmsg("testRecord");
	}

	free(line);
	free(r2.playerName);

}

void testRecordWithSpaces(bool showPassedTests)
{
	/* write the record */
	FILE *f = fopen("highscore-test.txt", "w+b");
	Record r = {.score=123, .timestamp=123456, 
		.playerName="name and number"};
	writeRecord(f, &r);
	fclose(f);

	/* read the record */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse the record */
	Record r2;
	parseLine(line, &r2);

	if (r2.score != r.score || r2.timestamp != r.timestamp || 
			strcmp(r2.playerName, r.playerName) != 0) {
		fmsg("testRecordWithSpaces, records differ");
	} else {
		pmsg("testRecord");
	}

	free(line);
	free(r2.playerName);

}

bool recordEqual(Record *r1, Record *r2) 
{
	return (r1->score == r2->score && r1->timestamp == r2->timestamp &&
			strcmp(r1->playerName, r2->playerName) == 0) == true;
}

void testHighscoreTable(bool showPassedTests)
{
	/* the highscoretable to write */
	HighscoreTable ht;

	/* create an array of records */
	Record records[6] = {
		[0]={.score=1, .timestamp=11, .playerName="1"},
		[1]={.score=2, .timestamp=12, .playerName="2"},
		[2]={.score=3, .timestamp=13, .playerName="3"},
		[3]={.score=4, .timestamp=14, .playerName="4"},
		[4]={.score=5, .timestamp=15, .playerName="5"},
		[5]={.score=6, .timestamp=16, .playerName="6"}};

	ht.records = records;
	ht.count = 6;
	ht.p = 6;

	if (writeHighscoreToFile("highscore-test.txt", &ht) != 0) {
		fmsg("testHighscoreTable, could not write highscore file.");
	}

	HighscoreTable *ht2 = loadHighscoreFromFile("highscore-test.txt");

	if (ht.count != ht2->count) {
		fmsg("testHighscoreTable, count not the same");
		fmsg(" is %d, should be %d\n", ht2->count, ht.count);
	} 

	/* if all the records are equal, this will be true after the loop */
	bool recordsEqual = true;

	/* compare the records */
	for (int i=0; i<ht.count; i++) {
		if (recordEqual(&ht.records[i], &ht2->records[i]) != true) {
			fmsg("testHighscoreTable, records differ");
			recordsEqual = false;
		}
	}

	if (recordsEqual == true) {
		pmsg("testHighscoreTable");
	}

	freeHighscoreTable(ht2);
}

void testAddRecord(bool showPassedTests)
{
	HighscoreTable ht;

	ht.records = (Record *) malloc(sizeof(Record)*2);
	Record r1 = {.score=1, .timestamp=11, .playerName="1"};
	Record r2 = {.score=2, .timestamp=12, .playerName="2"};
	ht.records[0] = r1;
	ht.records[1] = r2;

	ht.count = 2;
	ht.p = 2;

	Record r = {.score=3, .timestamp=13, .playerName="3"};

	if (addRecordToHighscoreTable(&r, &ht) != 0) {
		fmsg("testAddRecord, could not add record\n");
	} else {
		if (recordEqual(&r, &ht.records[2]) != true) {
			fmsg("testAddRecord, records differ");
		} else {
			pmsg("testAddRecord");
		}
	}

	free(ht.records);
}

void testInsertRecord(bool showPassedTests) 
{
	HighscoreTable ht;

	Record r1 = {.score=2, .timestamp=11, .playerName="1"};
	Record r2 = {.score=1, .timestamp=12, .playerName="2"};
	Record r3 = {.score=12, .timestamp=13, .playerName="3"};

	ht.records = (Record *) malloc(sizeof(Record)*2);
	ht.records[0] = r1;
	ht.records[1] = r2;

	ht.count = 2;
	ht.p = 2;

	insertRecordAndSort(&r3, &ht);

	if (recordEqual(&r3, &ht.records[0]) == false ||
			recordEqual(&r1, &ht.records[1]) == false ||
			recordEqual(&r2, &ht.records[2]) == false) {
		fmsg("testInsertRecord, records not sorted");
	} else {
		pmsg("testInsertRecord");
	}

	free(ht.records);

}
int main(int argc, char **argv)
{
	bool showPassedTests = false;
	if (argc > 1) {
		if (strcmp("--show-passed=yes", argv[1]) == 0) {
			showPassedTests = true;
		} else {
			showPassedTests = false;
		}
	}

	testMetadata(showPassedTests);
	testRecord(showPassedTests);
	testHighscoreTable(showPassedTests);
	testAddRecord(showPassedTests);
	testInsertRecord(showPassedTests);
	testRecordWithSpaces(showPassedTests);

	return 0;
}
