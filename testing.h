#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Print a message for a passed test.
 * If this macro is used, a variable `bool showPassedTests' must be set.
 * @param Something to put in printf
 */
#define pmsg(...) { \
	if (showPassedTests == true) { \
		char *msgline = malloc(1024); \
		snprintf(msgline, 1024, __VA_ARGS__); \
		printf("PASS: %s\n", msgline); \
		free(msgline); }} 

/**
 * Print a message for a failed test.
 * @param Something to put in printf
 */
#define fmsg(...) { \
	char *msgline = malloc(1024); \
	snprintf(&msgline, 1024, __VA_ARGS__); \
	fprintf(stderr, "TEST FAILURE: %s\n", msgline); \
	free(msgline); }

#endif
