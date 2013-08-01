#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef TESTING_H
#define TESTING_H

/**
 * Print a message for a passed test.
 * If this macro is used, a variable `bool showPassedTests' must be set.
 * @param Something to put in printf
 */
#define pmsg(...) { \
	if (showPassedTests == true) { \
		char *msgline = NULL; \
		asprintf(&msgline, __VA_ARGS__); \
		printf("PASS: %s\n", msgline); \
		free(msgline); }} 

/**
 * Print a message for a failed test.
 * @param Something to put in printf
 */
#define fmsg(...) { \
	char *msgline = NULL; \
	asprintf(&msgline, __VA_ARGS__); \
	fprintf(stderr, "TEST FAILURE: %s\n", msgline); \
	free(msgline); }

#endif
