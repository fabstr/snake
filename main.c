#include "main.h"

int main(int argc, char **argv) 
{
	/* initialization */
	initNCurses();

	if (COLS < 41 || LINES < 6) {
		endwin();
		fprintf(stderr, "The terminal window needs to be at least 41");
		fprintf(stderr, " chars wide and 6 lines high.\n");
		return 1;
	} 
	Board *b = initGame(COLS, LINES);
	if (b == NULL) {
		endwin();
		fprintf(stderr, "Could not initialize the board.\n");
		return 1;
	}

	/* the game loop */
	int toReturn = gameLoop(b);

	/* update highscore file */
	writeHighscoreToFile(highscorePath, b->highscore);

	/* memory deallocation */
	for (int row=0; row<b->height; row++) {
		if (b->segments[row] != NULL) {
			free(b->segments[row]);
		}
	}
	free(b->segments);
	freeHighscoreTable(b->highscore);
	free(b);

	/* pass through the exit status */
	return toReturn;
}

void mlog(char *msg)
{
	FILE *f = fopen("snake.log", "a+b");
	
	/* get the current timestamp and remove the newline */
	time_t currTime = time(NULL);
	char *timeString = ctime(&currTime);
	timeString[24] = '\0';

	/* print the message and close the file */
	fprintf(f, "%s: %s\n", timeString, msg);
	fclose(f);
}

Board* initGame(int width, int height)

{
	/* the snake board */
	Board *b = (Board *) malloc(sizeof(Board));

	/* set width and height */
	b->width = width;
	b->height = height-2;

	/* give the snake's head the head type */
	b->head.type = HEAD;

	/* initialize the segments array */
	b->segments = (Segment **) malloc(b->height*sizeof(Segment*));

	/* initialize the segments array */
	for (int i=0; i<b->height; i++) {
		b->segments[i] = (Segment *) malloc(b->width*sizeof(Segment));
	}

	/* load highscore */
	if ((b->highscore = loadHighscoreFromFile(highscorePath)) == NULL) {
		endwin();
		printf("highscore not loaded");
		return NULL;
	}

	/* use the reset function to init the rest */
	resetGame(b);

	return b;
}

void initNCurses()
{
	/*init ncurses*/
	initscr();

	/* use colours */
	start_color();

	/* get keys as they are pressed */
	cbreak();

	/* don't echo the keys as the are pressed */
	noecho();

	/* prevent flushing on interrupt keys */
	intrflush(stdscr, FALSE);

	/* prevent blocking on getch() */
	nodelay(stdscr, TRUE);

	/* enable keypad (and arrow keys) */
	keypad(stdscr, TRUE);

	/* hide the cursor */
	curs_set(0);

	/* initialize the colours */
	init_color(COLOR_BLUE, 0, 0, 999);
	init_pair(TEXT_COLOR_INDEX, COLOR_WHITE, COLOR_BLACK); /* wall/text */
	init_pair(BODY_COLOR_INDEX, COLOR_BLUE, COLOR_BLACK); /* body */
	init_pair(FOOD_COLOR_INDEX, COLOR_GREEN, COLOR_BLACK); /* food */
	init_pair(TEXT_INPUT_INDEX, COLOR_BLACK, COLOR_WHITE); /* text input */
}

void destroyOldBodySegments(Board *b)
{
	for (int row=0; row<b->height; row++) {
		for (int col=0; col<b->width; col++) {
			/*check if the segment should be removed*/
			if (b->segments[row][col].type == BODY) {

				/* first decrease the life ticks */
				b->segments[row][col].lifeTicks 
					-= LifeTicksDecreaseSpeed;

				/* then check if the segment should die */
				if (b->segments[row][col].lifeTicks <= 0) {
					b->segments[row][col].type = AIR;
					b->segments[row][col].blocking = false;
				}
			} 
		}
	}
}

void drawSnake(Board *b)
{
	/* draw the segments */
	int row, col;
	if (b->segments == NULL) {
		return;
	}

	for (row=0; row<b->height; row++) {
		if (b->segments[row] == NULL) {
			continue;
		}

		for (col=0; col<b->width; col++) {
			/* draw the block */
			if (b->segments[row][col].type != AIR) {
				Segment currSeg = b->segments[row][col];

				/* give the character the correct color */
				int charToWrite = currSeg.drawingCharacter;
				if (currSeg.type == BODY) {
					charToWrite |= 
						COLOR_PAIR(BODY_COLOR_INDEX);
				} else if (currSeg.type == FOOD) {
					charToWrite |= 
						COLOR_PAIR(FOOD_COLOR_INDEX);
				}

				mvaddch(currSeg.p.row, currSeg.p.column, 
					charToWrite);
			}
		}
	}

	/* draw the head */
	int headChar;
	switch (b->direction) {
		case LEFT:
			headChar = ACS_LARROW;
			break;
		case RIGHT:
			headChar = ACS_RARROW;
			break;
		case UP:
			headChar = ACS_UARROW;
			break;
		case DOWN:
			headChar = ACS_DARROW;
			break;
	}

	mvaddch(b->head.p.row, b->head.p.column, headChar | 
		COLOR_PAIR(BODY_COLOR_INDEX));
}

bool positionIsOccupied(Position p, Board *b)
{
	int row = p.row;
	int col = p.column;
	if (b->segments[row][col].type != AIR) {
		return true;
	}

	return false;
}

void drawBorder(int col1, int row1, int col2, int row2)
{
	/* draw a border */
	int r, c;

	/* top */
	for (r=row1,c=col1; c<col2; c++) {
		mvaddch(r, c,  ACS_HLINE | COLOR_PAIR(TEXT_COLOR_INDEX));
	}

	/* bottom */
	for (r=row2, c=col1; c<col2; c++) {
		mvaddch(r, c, ACS_HLINE | COLOR_PAIR(TEXT_COLOR_INDEX));
	}

	/* left */
	for (r=row1,c=col1; r<row2; r++) { 
		mvaddch(r, c, ACS_VLINE | COLOR_PAIR(TEXT_COLOR_INDEX));
	}

	/* right */
	for (r=row1,c=col2; r<row2; r++) { 
		mvaddch(r, c, ACS_VLINE | COLOR_PAIR(TEXT_COLOR_INDEX));
	}

	/* fix corners */
	mvaddch(row1, col1, ACS_ULCORNER | COLOR_PAIR(TEXT_COLOR_INDEX));
	mvaddch(row1, col2, ACS_URCORNER | COLOR_PAIR(TEXT_COLOR_INDEX));
	mvaddch(row2, col1, ACS_LLCORNER | COLOR_PAIR(TEXT_COLOR_INDEX));
	mvaddch(row2, col2, ACS_LRCORNER | COLOR_PAIR(TEXT_COLOR_INDEX));
}

void generateFood(Board *b)
{
	srandom(time(NULL));
	bool generating = true;
	while (generating) {
		/* get the limits for the border */
		int row = (int) random() % (b->height - 2);
		int col = (int) random() % (b->width - 2);
		row++; 
		col++;

		/* construct the current position */
		Position pos;
		pos.row = row;
		pos.column = col;

		if (positionIsOccupied(pos, b) == false) {
			Segment s;
			s.p = pos;
			s.type = FOOD;
			s.drawingCharacter = ACS_DIAMOND;
			b->segments[row][col] = s;
			break;
		}
	}
}

int getTextInput(char *msg, char *dest, size_t bufflen) 
{
	/* quit ncurses */
	endwin();

	/* show the message */
	printf("%s", msg);
	fflush(stdout);

	/* get the input */
	int toReturn = getline(&dest, &bufflen, stdin);

	/* trim the newline */
	dest[strlen(dest)-1] = '\0';
	
	/* start ncurses again */
	refresh();

	/* return something */
	if (toReturn <= 0) {
		return 1;
	} else {
		return 0;
	}
}

void lose(Board *b) 
{
	/* get the lowest score, the last one in the records array */
	int lowestScore = b->highscore->records[b->highscore->count-1].score;

	if (lowestScore < b->food) {
		/* get the name */
		char **text = (char **) malloc(3*sizeof(char **));
		text[0] = "                    YOU LOST!                 ";
		text[1] = "Please enter your name for the highscore list.";
		text[2] = "           Press space to continue.           ";

		drawTextWindowInMiddle(text, 3);
		refresh();

		char c;
		while ((c = getch()) != ' ') {
			usleep(10000);
		}

		/* get the user's name */
		char *name = (char *) malloc(64);
		if (getTextInput("What's your name? ", name, 64) != 0) {
			GameState = QUIT;
		}
		mlog("got line:");
		mlog(name);

		/* the player has beaten someone, add the player to the 
		 * highscore list */
		Record r = {.score = b->food, .timestamp = (long) time(NULL), 
			.playerName=name};
		insertRecordAndSort(&r, b->highscore);
		GameState = HIGHSCORE;
	} else {

	}


	resetGame(b);
}

bool headIsOutOfBoard(Board *b)
{
	/* the head's position */
	Position hp = b->head.p;

	if (hp.row < 1 || hp.row >= b->height-1 || hp.column < 1 ||
			hp.column >= b->width-1) {
		return true;
	}

	return false;
}

void moveSnakeHead(Board *b) {
	/* move the snake's head in the current moving direction */
	switch (b->direction) {
		case LEFT:
			b->head.p.column--;
			break;
		case UP:
			b->head.p.row--;
			break;
		case RIGHT:
			b->head.p.column++;
			break;
		case DOWN:
			b->head.p.row++;
			break;
	}
}

void getInput(Board *b) {
	/* save the old direction */
	b->previousDirection = b->direction;

	/* get the new direction */
	int ch = getch();
	switch (ch) {
		case 'a': 
		case KEY_LEFT:
			if (b->direction != RIGHT) {
				b->direction = LEFT;
			}
			break;
		case 'd': 
		case KEY_RIGHT:
			if (b->direction != LEFT) {
				b->direction = RIGHT;
			}
			break;
		case 's': 
		case KEY_DOWN:
			if (b->direction != UP) {
				b->direction = DOWN;
			}
			break;
		case 'w': 
		case KEY_UP:
			if (b->direction != DOWN) {
				b->direction = UP;
			}
			break;
		case 'p':
			GameState = (GameState == PAUSED) ? PLAYING : PAUSED;
			break;
		case 'h':
			GameState = (GameState == HELP) ? PLAYING : HELP;
			break;
		case 'q':
			GameState = QUIT;
			break;
		case 't':
			GameState = (GameState == HIGHSCORE) ?
				PLAYING : HIGHSCORE;
			break;
	}
}

void createBodySegmentAtHeadPosition(Board *b)
{
	/* set asegment at the heads position */
	Segment s;
	s.p.row = b->head.p.row;
	s.p.column = b->head.p.column;
	s.lifeTicks = SegmentLife;
	s.blocking = true;
	s.type = BODY;
	if (b->direction != b->previousDirection) {
		/* the snake is changing direction */
		if (b->previousDirection == RIGHT && b->direction == DOWN) {
			s.drawingCharacter = ACS_URCORNER;
		} else if (b->previousDirection == RIGHT && b->direction == UP) {
			s.drawingCharacter = ACS_LRCORNER;
		} else if (b->previousDirection == DOWN && b->direction == RIGHT) {
			s.drawingCharacter = ACS_LLCORNER;
		} else if (b->previousDirection == DOWN && b->direction == LEFT) {
			s.drawingCharacter = ACS_LRCORNER;
		} else if (b->previousDirection == LEFT && b->direction == UP) {
			s.drawingCharacter = ACS_LLCORNER;
		} else if (b->previousDirection == LEFT && b->direction == DOWN) {
			s.drawingCharacter = ACS_ULCORNER;
		} else if (b->previousDirection == UP && b->direction == LEFT) {
			s.drawingCharacter = ACS_URCORNER;
		} else if (b->previousDirection == UP && b->direction == RIGHT) {
			s.drawingCharacter = ACS_ULCORNER;
		}
	} else if (b->direction == UP || b->direction == DOWN) {
		s.drawingCharacter = ACS_VLINE;
	} else if (b->direction == LEFT || b->direction == RIGHT) {
		s.drawingCharacter = ACS_HLINE;
	}

	b->segments[b->head.p.row][b->head.p.column] = s;
}

void update(Board *b)
{

	/* update/move the snake */
	createBodySegmentAtHeadPosition(b);
	moveSnakeHead(b);
	destroyOldBodySegments(b);
}

bool hasPlayerLost(Board *b)
{
	/* check if the head has hit something */
	if (positionIsOccupied(b->head.p, b) == true) {
		/* there is something at the head */
		Segment currSeg = b->segments[b->head.p.row]
			[b->head.p.column];

		if (currSeg.type == FOOD) {
			/* we found food! */
			b->food++;
			generateFood(b);

			/* make the snake longer (by making the 
			 * segments live longer) */
			SegmentLife += LifeTicksDecreaseSpeed;
		} else if (currSeg.blocking == true) {
			/* there was something blocking, the snake is 
			 * dead! */
			return true;
		}
	}

	if (headIsOutOfBoard(b) == true) {
		return true;
	}

	return false;
}

void resetGame(Board *b)
{
	/* place the head at the middle */
	b->head.p.row = b->height/2;
	b->head.p.column = b->width/2;

	/* set movind direction */
	b->direction = UP;

	/* replace the segments array with air segments */
	for (int row=0; row<b->height; row++) {
		for (int col=0; col<b->width; col++) {
			struct timeval t;
			gettimeofday(&t, NULL);
			Segment s = {{row, col}, AIR, -1, false};
			b->segments[row][col] = s;
		}
	}

	/* reset the food */
	b->food = 0;

	/* add some food */
	generateFood(b);
}

void drawStats(Board *b)
{
	int score = b->food;
	char *scoreString, *helpString;
	asprintf(&scoreString, "Score: %10d ", score);
	asprintf(&helpString, " Press h for help.");

	mvaddstr(LINES-2, 2, scoreString);

	/* save the current position to write tees */
	int x, y;
	getyx(stdscr, y, x);

	/* write separator and tees */
	addch(ACS_VLINE | COLOR_PAIR(TEXT_COLOR_INDEX));
	mvaddch(y-1, x, ACS_TTEE | COLOR_PAIR(TEXT_COLOR_INDEX));
	mvaddch(y+1, x, ACS_BTEE | COLOR_PAIR(TEXT_COLOR_INDEX));

	/* write the help string */
	mvaddstr(y, x+1, helpString);

	free(scoreString);
	free(helpString);
}


void drawTextWindowInMiddle(char **text, int nrows)
{
	/* find the longest string */
	int longestLength = strlen(text[0]);
	int len = 0;
	for (int i=0; i<nrows; i++) {
		if ((len = strlen(text[i])) > longestLength) {
			longestLength = len;
		}
	}

	int xpos = COLS/2 - longestLength/2 - 1;
	int ypos = LINES/2 - nrows/2;

	/* draw the border */
	drawBorder(xpos, ypos, xpos+longestLength+1, ypos+nrows+1);

	/* draw the text */
	for (int i=0; i<nrows; i++) {
		/* ypos+0 and xpos+0 is the border */
		mvaddstr(ypos+i+1, xpos+1, text[i]);
	}
}

void drawHighscore(Board *b)
{
	/* move to center of screen */
	move(LINES/2, COLS/2);

	/* get the coordinates */
	int x, y;
	getyx(stdscr, y, x);

	/* the number of lines to draw */
	int nlines = nlines = b->highscore->count+1;
	if (nlines > 11) {
		nlines = 11;
	}

	/* make the array for printing */
	char **text = (char **) malloc(nlines*sizeof(char **));
	text[0] = "Top 10 (press t to continue):";

	for (int i=0; i<b->highscore->count && i < 10; i++) {
		Record currRec = b->highscore->records[i];

		/* in the text array we begin at index 1 */
		asprintf(&text[i+1], "%5d %s", currRec.score, 
				currRec.playerName);
	}

	/* draw the text */
	drawTextWindowInMiddle(text, nlines);

	/* free the strings */
	for (int i=1; i<nlines; i++) {
		free(text[i]);
	}
}

void drawHelp() 
{
	/* the length of the longest string */
	int longestStringLength = strlen("Use the arrow keys or WASD to move.");

	/* move to center of screen */
	move(LINES/2, COLS/2);

	/* get the coordinates */
	int x, y;
	getyx(stdscr, y, x);
	x -= longestStringLength/2 + 2;

	char *text[8] = {"Press p to pause.", 
		"Use the arrow keys or WASD to move.", 
		"Your head looks like this: ", 
		"Your body: + (don't eat it).", 
		"Food: O (eat it).", 
		"Press q to quit.",
		"Press t to show highscore.",
		"Press h to continue."};
			
	drawTextWindowInMiddle(text, 8);
}

/* return non-zero on error */
int draw(Board *b)
{
	/* clear the screen */
	if (erase() != OK) {
		endwin();
		fprintf(stderr, "Could not clear the window.\n");
		return 1;
	}

	/* draw the score window */
	drawBorder(0, LINES-3, COLS-1, LINES-1);
	drawStats(b);

	/* draw the border around everything */
	drawBorder(0, 0, COLS-1, LINES-1);

	/* draw the snake */
	drawSnake(b);

	char *textarr[2];
	switch (GameState) {
		case HELP:
			drawHelp();
			break;
		case HIGHSCORE:
			drawHighscore(b);
			break;
		case PAUSED:
			textarr[0] = "The game is paused.";
			textarr[1] = "Press p to continue.";
			drawTextWindowInMiddle(textarr, 2);
			break;
		case PLAYING: 
		case QUIT:
			break;
	}

	/*finally refresh the window and wait some time*/
	if (refresh() != OK) {
		endwin();
		fprintf(stderr, "Could not redraw the window.\n");
		return 1;
	}

	return 0;
}

int gameLoop(Board *b) 
{
	while (true) {
		/* get input from user */
		getInput(b);

		switch (GameState) {
			case PLAYING:
				/* update the game */
				update(b);
				/* check if the player has lost, this function
				 * also handles the food */
				if (hasPlayerLost(b) == true) {
					lose(b);
				}
				break;
			case QUIT:
				endwin();
				return 0;
			default:
				break;

		}

		if (draw(b) != 0) {
			/* there was an error */
			return 1;
		}

		/* wait */
		usleep(SleepingTime);
	}
}
