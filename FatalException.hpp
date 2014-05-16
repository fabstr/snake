#ifndef FATALEXCEPTION_H
#define FATALEXCEPTION_H

#include <cstdlib>
#include <string>

using namespace std;

class FatalException
{
private:
	string message;
	FatalException *e;

public:
	/**
	 * @param msg The message to print.
	 */
	FatalException(string msg);

	/**
	 * @param msg The message to print.
	 * @param e Another exception that caused this.
	 */
	FatalException(string msg, FatalException *e);

	string getMessage();

	~FatalException();
};

#endif
