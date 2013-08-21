#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

#include "mlog.h"

#ifndef CMDLINEARGS_H
#define CMDLINEARGS_H

/**
 * A command line option.
 * Examples: Calling with -a -b
 *   -a "foo" is encoded as {.argument="foo", .set=true}
 *   -b is encoded as {.option='b', .argument=NULL}
 *   -c (not supplied) is encoded as {.argument=NULL, .set=false}
 */
typedef struct Option {
	char *argument;
	bool set;
} Option;

typedef struct Options {
	Option remote;
	/**< @brief if set, listen for UP, LEFT, RIGHT and DOWN messages 
	 * Will listen on the given port. 
	 */

	Option port;
	/**< @brief The port to listen on */

	Option ai;
	/**< @brief let ai control instead of a human */

	Option growthSpeed;
	/**< @brief The speed the snake growth with, is 1, 2 or 3. */

	Option movementSpeed;
	/**< @brief The speed the snake moves with, is 1, 2 or 3. */
} Options;

/* the default port */
#define DEFAULTPORT "14590"

/**
 * Parse the command line options from argv 
 * @param argc argc as in main()
 * @param argv argv as in main()
 * @return An Option of the args.
 */
Options parseOptions(int argc, char **argv);

/**
 * Print the help/usage text.
 */
void printHelp();

/**
 * Reset the options, all set variables are set to false and the arguments are
 * set to NULL.
 * @param o The options to reset.
 */
void resetOptions(Options *o);
#endif
