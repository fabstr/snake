.PHONY: all run test run-tests wc top clean aitest

# the source files
SRCS = main.c snake.c highscore.c segment.c draw.c board.c position.c \
       mlog.c cmdlineargs.c protocol.c ai.c bheap.c astar.c
TESTSRCS = highscoreTest.c stackTest.c bheapTest.c

# the compiler flags
CFLAGS = -g -O0 -Wall 

# the libraries used
LDLIBS = -lncurses

# the c99 compiler
#CC = gcc -std=c99 # on non-osx
CC = clang

# the output file
BIN = snake

# the test output files
TESTBINS = highscoreTest bheapTest

# other files to clean
OTHERCLEANING = snake.log highscore-test.txt valgrind.log

# dSYM's to remove
DSYMS = $(BIN:=.dSYM) $(TESTBINS:=.dSYM)

# the dependency files
DEPS = $(SRCS:.c=.P)
TESTDEPS = $(TESTSRCS:.c=.P)

# to run the tests
TESTRUNS = $(TESTBINS:Test=-runTest)
TESTFLAGS = --show-passed=yes
TESTLIBS = mlog.o testing.o

# the valgrind flags
VLGDFLAGS = --leak-check=full --log-file=valgrind.log --track-origins=yes 

# the objects files
OBJECTS = $(SRCS:.c=.o)
TESTOBJECTS = $(TESTSRCS:.c=.o)

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(BIN) $(OBJECTS)

run: all
	./$(BIN)

run-valgrind: all $(BIN).dSYM
	valgrind $(VLGDFLAGS) ./$(BIN)

tests: $(TESTBINS)

run-tests: $(TESTRUNS)

run-tests-valgrind: $(TESTRUNS)

%-runTest: %Test
	./$*Test $(TESTFLAGS)

%-runTestVlgd: %Test %Test.dSYM
	valgrind $(VLGDFLAGS) %*Test

wc:
	@wc -l *.c *.h | grep -i -v -e tree.h -e queue.h -e total | awk '{print $$1}' | awk '{sum += $$1} END {print sum " total"}'

top:
	echo  -pid `ps -el | grep snake | grep -v grep | awk '{print $$2}'`

clean:
	@rm -f $(OBJECTS) $(TESTOBJECTS) # the object files
	@rm -f $(BIN) $(TESTBINS) # binary files
	@rm -f $(OTHERCLEANING) # other stuff
	@rm -f $(DEPS) $(TESTDEPS) # dependencies
	@rm -rf $(DSYMS) # debugging info

%.dSYM : % 
	dsymutil $* -o $*.dSYM

%.o : %.c
	$(COMPILE.c) -MD -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

%Test: %.o %Test.o $(TESTLIBS)
	$(CC) $(CFLAGS) $(TESTLIBS) $*Test.o $*.o -o $@

aitest: all
	rm -f snake.log
	valgrind snake -a
	vless snake.log
-include $(DEPS)
-include $(TESTDEPS)
