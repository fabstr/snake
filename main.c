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
	freeSnake(b->snake);
	freeHighscoreTable(b->highscore);
	free(b);

	/* pass through the exit status */
	return toReturn;
}

Board* initGame(int width, int height)

{
	/* the snake board */
	Board *b = (Board *) malloc(sizeof(Board));

	/* set width and height */
	b->width = width;
	b->height = height-2;

	/* load highscore */
	if ((b->highscore = loadHighscoreFromFile(highscorePath)) == NULL) {
		endwin();
		printf("highscore not loaded");
		return NULL;
	}

	/* setup the food segment */
	b->foodSegment.type = FOOD;
	b->foodSegment.drawingCharacter = ACS_DIAMOND;
	b->foodSegment.color_pair = COLOR_PAIR(FOOD_COLOR);

	/* use the reset function to init the rest */
	b->snake = NULL; /* will be set in resetGame */
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
	init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK); /* wall/text */
	init_pair(BODY_COLOR, COLOR_BLUE, COLOR_BLACK); /* body */
	init_pair(FOOD_COLOR, COLOR_GREEN, COLOR_BLACK); /* food */
	init_pair(TEXT_INPUT, COLOR_BLACK, COLOR_WHITE); /* text input */
	init_pair(HEAD_COLOR, COLOR_RED, COLOR_BLACK); /* text input */
}

void drawBorder(int col1, int row1, int col2, int row2)
{
	/* draw a border */
	int r, c;

	/* top */
	for (r=row1,c=col1; c<col2; c++) {
		mvaddch(r, c,  ACS_HLINE | COLOR_PAIR(TEXT_COLOR));
	}

	/* bottom */
	for (r=row2, c=col1; c<col2; c++) {
		mvaddch(r, c, ACS_HLINE | COLOR_PAIR(TEXT_COLOR));
	}

	/* left */
	for (r=row1,c=col1; r<row2; r++) { 
		mvaddch(r, c, ACS_VLINE | COLOR_PAIR(TEXT_COLOR));
	}

	/* right */
	for (r=row1,c=col2; r<row2; r++) { 
		mvaddch(r, c, ACS_VLINE | COLOR_PAIR(TEXT_COLOR));
	}

	/* fix corners */
	mvaddch(row1, col1, ACS_ULCORNER | COLOR_PAIR(TEXT_COLOR));
	mvaddch(row1, col2, ACS_URCORNER | COLOR_PAIR(TEXT_COLOR));
	mvaddch(row2, col1, ACS_LLCORNER | COLOR_PAIR(TEXT_COLOR));
	mvaddch(row2, col2, ACS_LRCORNER | COLOR_PAIR(TEXT_COLOR));
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
		Position pos = {.row=row, .column=col};

		if (isPositionOccupied(b->snake, pos) == false) {
			b->foodSegment.p = pos;
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

	if (lowestScore < b->snake->score) {
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

		/* the player has beaten someone, add the player to the 
		 * highscore list */
		Record r = {.score = b->snake->score, 
			.timestamp = (long)time(NULL), .playerName=name};
		insertRecordAndSort(&r, b->highscore);
		GameState = HIGHSCORE;
	} else {

	}


	resetGame(b);
}

bool headIsOutOfBoard(Board *b)
{
	/* the head's position */
	Position hp = b->snake->head.p;

	if (hp.row < 1 || hp.row >= b->height-1 || hp.column < 1 ||
			hp.column >= b->width-1) {
		return true;
	}

	return false;
}

void getInput(Board *b) {
	/* save the old direction */
	b->snake->previousDirection = b->snake->direction;

	/* get the new direction */
	int ch = getch();
	switch (ch) {
		case 'a': 
		case KEY_LEFT:
			if (b->snake->direction != RIGHT) {
				b->snake->direction = LEFT;
			}
			break;
		case 'd': 
		case KEY_RIGHT:
			if (b->snake->direction != LEFT) {
				b->snake->direction = RIGHT;
			}
			break;
		case 's': 
		case KEY_DOWN:
			if (b->snake->direction != UP) {
				b->snake->direction = DOWN;
			}
			break;
		case 'w': 
		case KEY_UP:
			if (b->snake->direction != DOWN) {
				b->snake->direction = UP;
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

void checkForFood(Board *b)
{
	if (positionEqual(b->foodSegment.p, b->snake->head.p) == true) {
		/* there is food here! 
		 *  - Increase the score
		 *  - Increase the living time (the length)
		 *  - add a segment at the head
		 *  - create more food*/
		b->snake->score++;
		b->snake->segmentLivingTime += SegmentLife;
		addSegmentAtHeadsPosition(b->snake);
		generateFood(b);
	}
}

void update(Board *b)
{
	checkForFood(b);
	updateSnake(b->snake);
}

bool hasPlayerLost(Board *b)
{
	/* check if the snake hits itself */
	if (isPositionOccupied(b->snake, b->snake->head.p) == true) {
		/* there is something at the head */
		return true;
	} else if (headIsOutOfBoard(b) == true) {
		return true;
	} else {
		return false;
	}
}

void resetGame(Board *b)
{
	/* replace the snake */
	if (b->snake != NULL) {
		freeSnake(b->snake);
	}

	b->snake = newSnake(b->height/2, b->width/2);

	/* add some food */
	generateFood(b);
}

void drawStats(Board *b)
{
	int score = b->snake->score;
	char *scoreString, *helpString;
	asprintf(&scoreString, "Score: %10d ", score);
	asprintf(&helpString, " Press h for help.");

	mvaddstr(LINES-2, 2, scoreString);

	/* save the current position to write tees */
	int x, y;
	getyx(stdscr, y, x);

	/* write separator and tees */
	addch(ACS_VLINE | COLOR_PAIR(TEXT_COLOR));
	mvaddch(y-1, x, ACS_TTEE | COLOR_PAIR(TEXT_COLOR));
	mvaddch(y+1, x, ACS_BTEE | COLOR_PAIR(TEXT_COLOR));

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

	/* draw the food */
	drawSegment(&b->foodSegment);

	/* draw the snake */
	drawSnake(b->snake);

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
