SRCS_NO_MAIN= FatalException.cpp board.cpp cmdlineargs.cpp draw.cpp highscore.cpp \
	mlog.cpp position.cpp protocol.cpp segment.cpp snake.cpp testing.cpp \
	highscoreMetadata.cpp record.cpp
SRCS=$(SRCS_NO_MAIN) main.cpp
SRCS_TEST=$(SRCS_NO_MAIN) highscoreTest.cpp
TARGET=snake
HIGHSCORETESTTARGET=hstest
OBJECTS=$(SRCS:.cpp=.o)
CXX=clang++
FLAGS=-Wall -g
LIBS=-lncurses
VLGDFLAGS=--leak-check=full --log-file=valgrind.log --track-origins=yes 

all: $(OBJECTS)
	$(CXX) -o $(TARGET) $(FLAGS) $(LIBS) $(OBJECTS)

hstest: $(SRCS_TEST:.cpp=.o) 
	$(CXX) -o $(HIGHSCORETESTTARGET) $(FLAGS) $(LIBS) $(SRCS_TEST:.cpp=.o) 

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

runlog: clean all
	rm -f snake.log
	screen ./snake
	less snake.log

runvalgrind: clean all $(TARGET).dSYM
	valgrind $(VLGDFLAGS) ./$(TARGET)
	
%.dSYM: %
	dsymutil $* -o $*.dSYM

clean: 
	rm -f *.o
	rm -f snake.log valgrind.log
	rm -f $(TARGET) 
	rm -rf $(TARGET).dSYM
	cp snakehighscore ~/.snakehighscore

