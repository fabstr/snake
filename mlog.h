#ifndef MLOG_H
#define MLOG_H

#include <stdio.h>
#include <time.h>

#define LOGFILE "snake.log"

#define mlog(...) {\
	char *logline = NULL; \
	asprintf(&logline, __VA_ARGS__); \
	write_log(logline); \
	free(logline); }

void write_log(char *msg);

#endif
