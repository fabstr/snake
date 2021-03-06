#include <stdio.h>
#include <time.h>

#ifndef MLOG_H
#define MLOG_H

#define LOGFILE "snake.log"

#define mlog(...) {\
	char *logline = malloc(1024); \
	snprintf(logline, 1024, __VA_ARGS__); \
	write_log(logline); \
	free(logline); \
}

void write_log(char *msg);

#endif
