#include "HighscoreMetadata.hpp"

HighscoreMetadata::HighscoreMetadata(string line)
{
	int count; 
	if (sscanf(line.c_str(), "Count=%d\n", &count) == 0) {
		throw new FatalException("Highscore metadata could not be loaded.");
	}

	this->count = count;
}

HighscoreMetadata::HighscoreMetadata(int c)
{
	count = c;
}


int HighscoreMetadata::writeMetadata(FILE *f)
{
	if (fprintf(f, "Count=%d\n", count) > 0) {
		return 0;

	} else {
		return 1;
	}
}
