#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <cstdlib>
#include <sstream>

#include "FatalException.hpp"

using namespace std;

class Record 
{
	int score;
	time_t timestamp;
	string playerName;

public:
	/**
	 * Constructor.
	 * Parse the line into a record.
	 * @param line The line to parse.
	 */
	Record(string line);

	/**
	 * Constructor.
	 * @param score The player's score.
	 * @param timestamp The time the player lost.
	 * @param playerName The player's name.
	 */
	Record(int score, long timestamp, string playerName);

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
	string toString();

	/**
	 * Get the score.
	 * @return The score.
	 */
	int getScore();

	/**
	 * Get the name.
	 * @return The name.
	 */
	string getName();

	/**
	 * Get the timestamp.
	 * @return The timestamp.
	 */
	long getTimestamp();
};

#endif
