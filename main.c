#include "main.h"

int main(int argc, char **argv) 
{
	/* for when the window is resized */
	/*signal(SIGWINCH, resizeBoard);*/

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

/*void resizeBoard(int i)*/
/*{*/
	/*windowIsResized = true;*/
/*}*/

void setBoardWidthHeight(Board *b, int windowCols, int windowRows)
{
	/* set width and height */
	b->width = windowCols;
	b->height = windowRows;
}

Board* initGame(int width, int height)

{
	/* the snake board */
	Board *b = (Board *) malloc(sizeof(Board));

	setBoardWidthHeight(b, COLS, LINES-2);

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

int gameLoop(Board *b) 
{
	while (true) {
		/* check if the window is resized */
		/*if (windowIsResized == true) {*/
			/*windowIsResized = false;*/
			/*setBoardWidthHeight(b, COLS, LINES-2);*/
		/*}*/

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
