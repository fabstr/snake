#include "highscore.hpp"

HighscoreTable::HighscoreTable(string file)
{
	/* open the highscore file */
	FILE *f = fopen(file.c_str(), "rb");

	/* to hold a line of the file */
	char *line = NULL;

	/* read the first line */
	size_t linecap = 0;
	getline(&line, &linecap, f);

	/* first read the metadata (the number of records) */
	HighscoreMetadata *metadata;
	try {
		metadata = new HighscoreMetadata(string(line));
	} catch (FatalException *e) {
		fclose(f);
		throw new FatalException("Could not load metadata.", e);
	}

	/* set the size of the array */
	count = metadata->count;

	/* make the entries array fit the number of records */
	records = new Record*[count];

	/* read the lines and parse them */
	int p = 0; /* the index of the entries array */
	while (getline(&line, &linecap, f) > 0) {
		// parse the record from the line
		Record *r;
		try {
			r = new Record(string(line));
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
void HighscoreTable::writeHighscoreTable(string file)
{
	FILE *f = fopen(file.c_str(), "w+b");

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

	delete[] records;
}

void HighscoreTable::addRecord(Record *r)
{
	if (p >= count) {
		/* we need to increase the size of the records array */
		size_t newSize = count+1;

		Record **newRecords = new Record*[newSize];
		for (int i=0; i<count; i++) {
			newRecords[i] = records[i];
		}

		delete[] records;
		records = newRecords;
	}

	/* add the record and update p/count */
	records[p] = r;
	p++;
	count++;
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

int HighscoreTable::getCount()
{
	return count;
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
				currRec->getName().c_str());
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
HighscoreTable::HighscoreTable(Record **records, int count, int p)
{
	this->records = records;
	this->count = count;
	this->p = p;
}
Record *HighscoreTable::getLowest()
{
	return records[count-1];
}

Record **HighscoreTable::getRecords()
{
	return records;
}
