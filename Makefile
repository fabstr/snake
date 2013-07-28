P=snake
OBJECTS=snake.c snake.h 
CFLAGS = -g -Wall -O0 
LDLIBS= -lncurses
CC=clang

#$(P): $(OBJECTS)

snake: snake.c snake.h
	clang -g -Wall -O0 -lncurses snake.c -o snake
