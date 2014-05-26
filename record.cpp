#include "Record.hpp"


Record::Record(string line)
{
	char *pn = (char *) malloc(64);
	if (pn == NULL) {
		throw new FatalException("Could not allocate memory for player name.");
	}

	if (sscanf(line.c_str(), "Score=%d Time=%ld Name=%64[0-9A-Za-z ]s\n",
				&score, &timestamp, pn) <= 0) {
		free(pn);
		throw new FatalException("Could not load highscore line.");
	}
	free(pn);
	playerName = string(pn);
}

Record::~Record()
{
}

string Record::toString()
{
	ostringstream str;
	str << "Score=" << score << " ";
	str << "Time=" << timestamp << " ";
	str << "Name=" << playerName;

	return str.str();
}

void Record::writeRecord(FILE *f)
{
	string line = this->toString();
	if (fprintf(f, "%s\n", line.c_str()) < 0) {
		throw new FatalException("Could not write a record.");
	} 
}

int Record::getScore()
{
	return score;
}


Record::Record(int score, long timestamp, string playerName)
{
	this->score = score;
	this->timestamp = timestamp;
	this->playerName = playerName;
}



string Record::getName()
{
	return playerName;
}

long Record::getTimestamp()
{
	return timestamp;
}


