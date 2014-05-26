#define _WITH_GETLINE
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "draw.hpp"
#include "FatalException.hpp"
#include "highscoreMetadata.hpp"
#include "record.hpp"

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

class HighscoreTable 
{
	Record **records; /* the array of the records */
	int count; /* the total number of records */
	int p; /* the latest non-occupied position of the array */

public:
	/**
	 * Constructor.
	 * Load the highscore from the given file.
	 * @param file The file to load from.
	 */
	HighscoreTable(string file);

	/**
	 * Constructor.
	 * The caller is responsible for the loading of the highscore table.
	 * @param records
	 * @param count
	 * @param p
	 */
	HighscoreTable(Record **records, int count, int p);

	/**
	 * Destructor.
	 */
	~HighscoreTable();

	/**
	 * Write the highscore to a file.
	 * @param file The path to the file to write to.
	 */
	void writeHighscoreTable(string file);

	/**
	 * Add a record.
	 * @param r The record to add.
	 */
	void addRecord(Record *r);

	/**
	 * Sort the records.
	 */
	void sort();

	/**
	 * Get the number of records.
	 * @return The count.
	 */
	int getCount();

	/**
	 * Get the records with the lowest score.
	 * @return The record.
	 */
	Record *getLowest();

	/**
	 * Draw the highscore table on screen.
	 */
	void draw();

	/**
	 * Return the array of records.
	 */
	Record **getRecords();
};

/**
 * Compare the Records e1 and e2.
 * If e1's score < e2, return 1.
 * If e1's score = e2, return 0.
 * If e1's score > e2, return -1.
 */
int compareRecords(const void *e1, const void *e2);

#endif
