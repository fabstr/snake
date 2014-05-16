#include "FatalException.hpp"

FatalException::FatalException(string msg)
{
	this->message = msg;
	e = NULL;
}

FatalException::FatalException(string msg, FatalException *e)
{
	this->message = msg;
	this->e = e;
}

string FatalException::getMessage()
{
	return this->message;
}

FatalException::~FatalException()
{
	if (e != NULL) {
		delete e;
	}
}
