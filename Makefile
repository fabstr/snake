# the flags for the compiler
CFLAGS= -g -O0 -Wall

# the flags for valgrind
VALGRINDFLAGS= --leak-check=full --log-file=valgrind.log --track-origins=yes

# the libraries used
LDLIBS= -lncurses

# the c99 compiler
CC= clang

# the objects to be compiled
OBJECTS= highscore.o snake.o

# the binary output
BIN= snake

# the test files to clean
TESTCLEAN= highscoreTest highscoreTest.o highscore-test.txt highscoreTest.dSYM valgrind.log

# other files to remove whilst cleaning
OTHERCLEANING= snake.log

all: snake

snake: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(BIN) $(OBJECTS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.dSYM : % 
	dsymutil $* -o $*.dSYM

clean:
	rm -rf $(OBJECTS) $(BIN) $(OTHERCLEANING) $(TESTCLEAN)

highscoreTest: highscore.o highscoreTest.o
	$(CC) $(CFLAGS) $(LDLIBS) highscoreTest.o highscore.o -o highscoreTest

test: clean highscoreTest 
	./highscoreTest

test-valgrind: clean highscoreTest.dSYM
	valgrind $(VALGRINDFLAGS) highscoreTest

run: all
	./snake

run-valgrind: clean snake.dSYM
	valgrind $(VALGRINDFLAGS) snake
