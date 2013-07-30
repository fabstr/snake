# the flags for the compiler
CFLAGS= -g -O0 -Wall

# the flags for valgrind
VALGRINDFLAGS= --leak-check=full --log-file=valgrind.log --track-origins=yes 

# the flags for the testing
TESTFLAGS= --show-passed=no

# the libraries used
LDLIBS= -lncurses

# the c99 compiler
CC= clang

# the objects to be compiled
OBJECTS= highscore.o main.o

# the binary output
BIN= snake

# the test files to clean
TESTCLEAN= highscoreTest highscoreTest.o highscore-test.txt highscoreTest.dSYM valgrind.log *.dSYM

# other files to remove whilst cleaning
OTHERCLEANING= snake.log

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(BIN) $(OBJECTS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.dSYM : % 
	dsymutil $* -o $*.dSYM

clean:
	rm -rf $(OBJECTS) 
	rm -rf $(OTHERCLEANING) 
	rm -rf $(TESTCLEAN)

highscoreTest: highscore.o highscoreTest.o
	$(CC) $(CFLAGS) $(LDLIBS) highscoreTest.o highscore.o -o highscoreTest

test: clean highscoreTest 
	./highscoreTest $(TESTFLAGS)

test-valgrind: clean highscoreTest.dSYM
	valgrind $(VALGRINDFLAGS) highscoreTest

run: clean all
	./snake

run-valgrind: clean snake.dSYM
	valgrind $(VALGRINDFLAGS) snake
