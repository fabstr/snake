#define _WITH_GETLINE
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "draw.hpp"
#include "FatalException.hpp"

#ifndef HIGHSCORE_H
#define HIGHSCORE_H


class Record 
{
	int score;
	time_t timestamp;
	char *playerName;

public:
	/**
	 * Constructor.
	 * Parse the line into a record.
	 * @param line The line to parse.
	 */
	Record(char *line);

	/**
	 * Constructor.
	 * @param score The player's score.
	 * @param timestamp The time the player lost.
	 * @param playerName The player's name.
	 */
	Record(int score, long timestamp, char *playerName);

	/**
	 * Destructor.
	 */
	~Record();

	/**
	 * Write the record to a file.
	 * @param f The file to write to.
	 */
	void writeRecord(FILE *f);

	/**
	 * Return the record as a string that can be read by the constructor.
	 * @return The string.
	 */
	string *toString();

	/**
	 * Get the score.
	 * @return The score.
	 */
	int getScore();

	/**
	 * Get the name.
	 * @return The name.
	 */
	char *getName();
};

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
	HighscoreTable(string *file);

	/**
	 * Destructor.
	 */
	~HighscoreTable();

	/**
	 * Write the highscore to a file.
	 * @param file The path to the file to write to.
	 */
	void writeHighscoreTable(char *file);

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
};

class HighscoreMetadata 
{
public:
	int count;

	/**
	 * Constructor.
	 * Read the metadata from the line
	 * @param line The line of metadata
	 */
	HighscoreMetadata(char *line);

	/**
	 * Constructor.
	 * Set the count from a parameter.
	 * @param c The count.
	 */
	HighscoreMetadata(int c);

	/**
	 * Write the metadata to a file.
	 * @param f The file to write to.
	 */
	int writeMetadata(FILE *f);
};

/**
 * Compare the Records e1 and e2.
 * If e1's score < e2, return 1.
 * If e1's score = e2, return 0.
 * If e1's score > e2, return -1.
 */
int compareRecords(const void *e1, const void *e2);

#endif
