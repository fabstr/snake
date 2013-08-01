# the flags for the compiler
CFLAGS= -g -O0 -Wall

# the flags for valgrind
VALGRINDFLAGS= --leak-check=full --log-file=valgrind.log --track-origins=yes 

# the flags for the testing
TESTFLAGS= --show-passed=no

# the libraries used
BINLDLIBS= -lncurses

# the c99 compiler
CC= clang

# the objects to be compiled
OBJECTS= highscore.o main.o snake.o segment.o colors.o draw.o board.o cmdlineargs.o stack.o position.o mlog.o
TESTOBJECTS= highscoreTest.o stackTest.o cmdlineargsTest.o
TESTUTILITIES= testing.o mlog.o

# the binary output
BIN= snake
TESTBINS= highscoreTest stackTest cmdlineargsTest

# to run the tests
TESTRUNS= $(TESTBINS:Test=TestRun)
TESTRUNVALGRIND= $(TESTBINS:Test=TestRunValgrind)
TESTDSYMS= $(TESTOBJECTS:.o=.dSYM)
DSYMS= $(OBJECTS:.o=.dSYM)

# the test files to clean
TESTCLEAN= $(TESTOBJECTS) $(TESTBINS) $(TESTDSYMS) $(TESTUTILITIES) highscore-test.txt valgrind.log 

# other files to remove whilst cleaning
OTHERCLEANING= snake.log snake.dSYM

snake: $(OBJECTS)
	$(CC) $(CFLAGS) $(BINLDLIBS) -o $(BIN) $(OBJECTS)

test: $(TESTRUNS)

testValgrind: $(TESTRUNVALGRIND)

run: snake
	./snake

run-valgrind: snake.dSYM
	valgrind $(VALGRINDFLAGS) snake 

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.dSYM : % 
	dsymutil $* -o $*.dSYM

clean:
	rm -rf $(OBJECTS) 
	rm -rf $(OTHERCLEANING) 
	rm -rf $(TESTCLEAN)

%Test : %.o %Test.c %Test.h $(TESTUTILITIES)
	$(CC) $(CFLAGS) $(LDLIBS) $*Test.c $*.o $(TESTUTILITIES) -o $*Test

%TestRun : %Test
	./$*Test $(TESTFLAGS)

%TestRunValgrind : %Test $(TESTDSYMS)
	valgrind $(VALGRINDFLAGS) $*Test

wc:
	wc -l *.c *.h | grep total

show-log:
	if [ -f snake.log ]; then less snake.log; fi

show-valgrind:
	if [ -f valgrind.log ]; then less valgrind.log; fi

debug: snake.dSYM
	cgdb -p `ps -el | grep snake | grep -v grep | awk '{print $$2}'`
