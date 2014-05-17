#include "highscore.hpp"

HighscoreMetadata::HighscoreMetadata(string *line)
{
	int count; 
	if (sscanf(line->c_str(), "Count=%d\n", &count) == 0) {
		throw new FatalException("Highscore metadata could not be loaded.");
	}

	this->count = count;
}

HighscoreMetadata::HighscoreMetadata(int c)
{
	count = c;
}

HighscoreTable::HighscoreTable(string *file)
{
	/* open the highscore file */
	FILE *f = fopen(file->c_str(), "rb");

	/* to hold a line of the file */
	char *line = NULL;

	/* read the first line */
	size_t linecap = 0;
	getline(&line, &linecap, f);

	/* first read the metadata (the number of records) */
	HighscoreMetadata *metadata;
	try {
		string l = string(line);
		metadata = new HighscoreMetadata(&l);
	} catch (FatalException *e) {
		fclose(f);
		throw new FatalException("Could not load metadata.", e);
	}

	/* make the entries array fit the number of records */
	records = (Record **) malloc(sizeof(Record *));

	/* set the size of the array */
	count = metadata->count;

	/* read the lines and parse them */
	int p = 0; /* the index of the entries array */
	while (getline(&line, &linecap, f) > 0) {
		// parse the record from the line
		Record *r;
		try {
			string l = string(line);
			r = new Record(&l);
		} catch (FatalException *e) {
			fclose(f);
			throw new FatalException("Could not load highscore, a line could not be loaded.", e);
		}

		// and add it to the array
		records[p++] = r;
	}

	this->p = p;

	free(line);
	delete metadata;
	fclose(f);
}

Record::Record(string *line)
{
	char *pn = (char *) malloc(64);
	if (pn == NULL) {
		throw new FatalException("Could not allocate memory for player name.");
	}

	if (sscanf(line->c_str(), "Score=%d Time=%ld Name=%64[0-9A-Za-z ]s\n",
				&score, &timestamp, pn) <= 0) {
		free(pn);
		throw new FatalException("Could not load highscore line.");
	}
	free(pn);
	playerName = new string(pn);
}

Record::~Record()
{
	free(playerName);
}

int HighscoreMetadata::writeMetadata(FILE *f)
{
	if (fprintf(f, "Count=%d\n", count) > 0) {
		return 0;

	} else {
		return 1;
	}
}

string *Record::toString()
{
	char *line;
	if (asprintf(&line, "Score=%d Time=%ld Name=%s", score, 
				timestamp, playerName->c_str()) == -1) {
		return NULL;
	}

	string *s = new string((const char *) line);
	free(line);
	return s;
}

void Record::writeRecord(FILE *f)
{
	string *line = this->toString();
	if (fprintf(f, "%s\n", line->c_str()) < 0) {
		throw new FatalException("Could not write a record.");
	} 
	delete line;
}

void HighscoreTable::writeHighscoreTable(string *file)
{
	FILE *f = fopen(file->c_str(), "w+b");

	HighscoreMetadata m(count);
	m.count = count;
	if (m.writeMetadata(f) != 0) {
		throw new FatalException("Could not write metadata.");
	}

	/* the position in the array of records */
	int p = 0;

	/* write every entry to file */
	while (p < this->p) {
		Record *r = records[p];
		r->writeRecord(f);
		p++;
	}

	fclose(f);
}

HighscoreTable::~HighscoreTable() 
{
	for (int i=0; i<count; i++) {
		delete records[i];
	}

	free(records);
}

void HighscoreTable::addRecord(Record *r)
{
	if (p >= count) {
		/* we need to increase the size of the records array */
		size_t newSize = count+1;

		if ((records = (Record **) realloc(records, newSize)) == NULL) {
			throw new FatalException("Could not reallocate memory for the highscore table.");
		}
	}

	/* add the record and update p/count */
	records[p] = r;
	p++;
	count++;
}

int Record::getScore()
{
	return score;
}

int compareRecords(const void *e1, const void *e2)
{
	Record *r1 = (Record *) e1;
	Record *r2 = (Record *) e2;

	if (r1->getScore() < r2->getScore()) {
		return 1;
	} else if (r1->getScore() == r2->getScore()) {
		return 0;
	} else {
		return -1;
	}
}

void HighscoreTable::sort()
{
	/* sort the records */
	qsort(records, p, sizeof(Record *), compareRecords);
}

Record::Record(int score, long timestamp, string *playerName)
{
	this->score = score;
	this->timestamp = timestamp;
	this->playerName = playerName;
}

int HighscoreTable::getCount()
{
	return count;
}

Record *HighscoreTable::getLowest()
{
	return records[count-1];
}

void HighscoreTable::draw()
{
	mlog("drawing highscore");

	/* the number of lines to draw */
	int nlines = nlines = count+1;
	if (nlines > 11) {
		nlines = 11;
	}

	mlog("have %d lines", nlines);

	/* make the array for printing */
	string *text = new string[nlines];
	text[0] = "Top 10 (press t to continue):";

	for (int i=0; i<count && i < 10; i++) {
		Record *currRec = records[i];

		/* in the text array we begin at index 1 */
		char *line; 
		asprintf(&line, "%2d. %4d %s", i+1, currRec->getScore(),
				currRec->getName()->c_str());
		text[i] = string(line);
		mlog("adding line %s", line);
		free(line);
	}

	/* draw the text */
	drawTextWindowInMiddle(text, nlines);

	/* free the strings */
	mlog("deleting text");
	delete[] text;
	mlog("done drawing highscore");

}

string *Record::getName()
{
	return playerName;
}
