SRCS= FatalException.cpp board.cpp cmdlineargs.cpp draw.cpp highscore.cpp main.cpp mlog.cpp position.cpp protocol.cpp segment.cpp snake.cpp testing.cpp
TARGET=snake
OBJECTS=$(SRCS:.cpp=.o)
CXX=clang++
FLAGS=-Wall 
LIBS=-lncurses
VLGDFLAGS=--leak-check=full --log-file=valgrind.log --track-origins=yes 

all: $(OBJECTS)
	$(CXX) -o $(TARGET) $(FLAGS) $(LIBS) $(OBJECTS)

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

