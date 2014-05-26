#include "highscoreTest.hpp"

void testRecordEqual(bool showPassedTests) 
{
	Record *r1 = new Record(1, 2, "3");
	Record *r2 = new Record(1, 2, "3");
	if (recordEqual(r1, r2) == false) {
		fmsg("testRecordEqual, r1 != r2");
	} else {
		pmsg("testRecordEqual");
	}
}

void testMetadata(bool showPassedTests)
{
	/* write to file */
	FILE *f = fopen("highscore-test.txt", "w+b");
	HighscoreMetadata *hm = new HighscoreMetadata(3);
	hm->writeMetadata(f);
	fclose(f);
	delete hm;

	/* read from file  */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse */
	string l = string(line);
	hm = new HighscoreMetadata(l);
	free(line);

	/* check */
	if (hm->count != 3) {
		fmsg("testMetadata, count != 3, is %d", hm->count);
	} else {
		pmsg("testMetadata");
	}

	delete hm;
}

void testRecord(bool showPassedTests)
{
	/* write the record */
	FILE *f = fopen("highscore-test.txt", "w+b");
	Record r = Record(123, (long) 123456, "name");
	r.writeRecord(f);
	fclose(f);

	/* read the record */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse the record */
	Record r2 = Record(string(line));

	if (r2.getScore() != r.getScore() || 
			r2.getTimestamp() != r.getTimestamp() || 
			strcmp(r2.getName().c_str(), r.getName().c_str()) != 0) {
		fmsg("testRecord, records differ");
	} else {
		pmsg("testRecord");
	}

	free(line);
}

void testRecordWithSpaces(bool showPassedTests)
{
	/* write the record */
	FILE *f = fopen("highscore-test.txt", "w+b");
	Record r = Record(123, (long) 123456, "name and number");
	r.writeRecord(f);
	fclose(f);

	/* read the record */
	FILE *g = fopen("highscore-test.txt", "rb");
	char *line = NULL;
	size_t linecap = 0;
	getline(&line, &linecap, g);
	fclose(g);

	/* parse the record */
	Record r2 = Record(string(line));

	if (r2.getScore() != r.getScore() || 
			r2.getTimestamp() != r.getTimestamp() || 
			strcmp(r2.getName().c_str(), r.getName().c_str()) != 0) {
		fmsg("testRecordWithSpaces, records differ");
	} else {
		pmsg("testRecord");
	}

	free(line);
}

bool recordEqual(Record *r1, Record *r2) 
{
	if (r1->getScore() == r2->getScore() &&
		r1->getTimestamp() == r2->getTimestamp() &&
		strcmp(r1->getName().c_str(), r2->getName().c_str()) == 0) {
		return true;
	}
	
	return false;
}

void testHighscoreTable(bool showPassedTests)
{
	/* create an array of records */
	Record **records = new Record*[6];
	records[0] = new Record(1, 11, "1");
	records[1] = new Record(2, 12, "2");
	records[2] = new Record(3, 13, "3");
	records[3] = new Record(4, 14, "4");
	records[4] = new Record(5, 15, "5");
	records[5] = new Record(6, 16, "6");

	HighscoreTable ht = HighscoreTable(records, 6, 6);

	ht.writeHighscoreTable("highscore-test.txt");

	HighscoreTable ht2 = HighscoreTable("highscore-test.txt");

	if (ht.getCount() != ht2.getCount()) {
		fmsg("testHighscoreTable, count not the same");
		fmsg(" is %d, should be %d\n", ht2.getCount(), ht.getCount());
	} 

	/* if all the records are equal, this will be true after the loop */
	bool recordsEqual = true;

	/* compare the records */
	for (int i=0; i<ht.getCount(); i++) {
		if (recordEqual(ht.getRecords()[i], ht2.getRecords()[i]) != true) {
			fmsg("testHighscoreTable, records differ");
			recordsEqual = false;
		}
	}

	if (recordsEqual == true) {
		pmsg("testHighscoreTable");
	}
}

void testAddRecord(bool showPassedTests)
{
	Record **arr = new Record*[2];
	arr[0] = new Record(1, 11, "1");
	arr[1] = new Record(2, 12, "1");
	
	HighscoreTable ht = HighscoreTable(arr, 2, 2);

	Record *r = new Record(3, 13, "3");

	ht.addRecord(r);
	if (recordEqual(r, ht.getRecords()[2]) != true) {
		fmsg("testAddRecord, records differ");
	} else {
		pmsg("testAddRecord");
	}
}

void testInsertRecord(bool showPassedTests) 
{

	Record *r1 = new Record(2, 11, "1");
	Record *r2 = new Record(1, 12, "2");
	Record *r3 = new Record(12, 13, "3");

	Record **arr = new Record*[2];
	arr[0] = r1;
	arr[1] = r2;
	HighscoreTable ht = HighscoreTable(arr, 2, 2);

	ht.addRecord(r3);
	ht.sort();

	Record **r = ht.getRecords();
	if (recordEqual(r3, r[0]) == false) {
		fmsg("testInsertRecord, records not sorted, r3 isn't first");
	} else if (recordEqual(r2, r[1]) == false) {
		fmsg("testInsertRecord, records not sorted, r2 isn't second");
	} else if (recordEqual(r1, r[2]) == false) {
		fmsg("testInsertRecord, records not sorted, r1 isn't last");
	} else {
		pmsg("testInsertRecord");
	}
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

	testRecordEqual(showPassedTests);
	testMetadata(showPassedTests);
	testRecord(showPassedTests);
	testHighscoreTable(showPassedTests);
	testAddRecord(showPassedTests);
	testInsertRecord(showPassedTests);
	testRecordWithSpaces(showPassedTests);

	return 0;
}
