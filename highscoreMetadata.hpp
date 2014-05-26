#ifndef HIGHSCORE_METADATA
#define HIGHSCORE_METADATA

#include <cstdio>
#include <string>

#include "FatalException.hpp"

using namespace std;

class HighscoreMetadata 
{
public:
	int count;

	/**
	 * Constructor.
	 * Read the metadata from the line
	 * @param line The line of metadata
	 */
	HighscoreMetadata(string line);

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

#endif
