#include "main.h"

int main(int argc, char **argv) 
{
	Options o = parseOptions(argc, argv);

	/* for when the window is resized */
	/*signal(SIGWINCH, resizeBoard);*/
	char *homePath = getenv("HOME");
	char *highscorePath = malloc(1024);
	snprintf(highscorePath, 1024, "%s/%s", homePath, HIGHSCORE_FILE);

	initNCurses(o.color.argument);

	if (COLS < 41 || LINES < 6) {
		endwin();
		fprintf(stderr, "The terminal window needs to be at least 41");
		fprintf(stderr, " chars wide and 6 lines high.\n");
		exit(EXIT_FAILURE);
	} 


	Board *b = initGame(COLS, LINES, highscorePath);
	if (b == NULL) {
		endwin();
		fprintf(stderr, "Could not initialize the board.\n");
		exit(EXIT_FAILURE);
	}

	if (o.remote.set == true) {
		mlog("opening connection at %s", o.port.argument);
		b->listenConnection = openListenConnection(o.port.argument);
		if (b->listenConnection == NULL) {
			fprintf(stderr, "Could not open socket to listen: %s",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		b->remote = true;
		mlog("connection opened");
	} else {
		if (o.ai.set == true) {
			mlog("ai steering");
			b->ai = true;
			/*init_ai(b->width, b->height);*/
		} else {
			mlog("local human steering");
			b->ai = false;
			b->listenConnection = NULL;
			b->remote = false;
		}
	}

	if (o.growthSpeed.set != true) {
		mlog("setting grow speed to 1");
		o.growthSpeed.argument = "1";
		o.growthSpeed.set = true;
	}

	if (o.movementSpeed.set != true) {
		o.movementSpeed.argument = "1";
		o.movementSpeed.set = true;
	}

	b->snake->growthSpeed = atoi(o.growthSpeed.argument);
	/*b->movementSpeed = atoi(o.movementSpeed.argument);*/
	mlog("growthspeed = %d", b->snake->growthSpeed);

	/* run the gaem */
	int toReturn = gameLoop(b);

	writeHighscoreToFile(highscorePath, b->highscore);


	/* memory deallocation */
	freeSnake(b->snake);
	freeHighscoreTable(b->highscore);
	free(highscorePath);
	if (b->listenConnection != NULL) {
		freeConnection(b->listenConnection);
	}
	free(b);

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

Board* initGame(int width, int height, char *highscorePath)

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

	b->snake = newSnake(b->width/2, b->height/2);
	/* use the reset function to init the rest */
	resetGame(b);

	return b;
}

void initNCurses(char *snakeColor)
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
	int bodycolor = BODY_COLOR;
	if (snakeColor == NULL) {
		bodycolor = BODY_COLOR;
        } else if (strcmp(snakeColor, "red") == 0) {
		bodycolor = COLOR_RED;
	} else if (strcmp(snakeColor, "green") == 0) {
		bodycolor = COLOR_GREEN;
	} else if (strcmp(snakeColor, "blue") == 0) {
		bodycolor = COLOR_BLUE;
	} else if (strcmp(snakeColor, "white") == 0) {
		bodycolor = COLOR_WHITE;
	} else if (strcmp(snakeColor, "yellow") == 0) {
		bodycolor = COLOR_YELLOW;
	}
	init_color(COLOR_BLUE, 0, 0, 999);
	init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK); /* wall/text */
	init_pair(BODY_COLOR, bodycolor, COLOR_BLACK); /* body */
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

void lose(Board *b, State *GameState)
{
	mlog("score: %d", b->snake->score);

	/* get the lowest score, the last one in the records array */
	int lowestScore;
	if (b->highscore->count >= 10) {
		lowestScore = b->highscore->records[9].score;
	} else {
		lowestScore = b->highscore->records[b->highscore->count-1].score;
	}

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
			*GameState = QUIT;
		}

		/* the player has beaten someone, add the player to the 
		 * highscore list */
		Record r = {.score = b->snake->score, 
			.timestamp = (long)time(NULL), .playerName=name};
		insertRecordAndSort(&r, b->highscore);
		*GameState = HIGHSCORE;
		free(text);
	} else {
		char **text = (char **) malloc(2*sizeof(char **));
		text[0] = "       YOU LOST!       ";
		text[1] = "Press space to continue.";

		drawTextWindowInMiddle(text, 2);
		refresh();

		char c;
		while ((c = getch()) != ' ') {
			usleep(10000);
		}

		free(text);
	}

	resetGame(b);
}

int gameLoop(Board *b) 
{
	State GameState = PLAYING;
	while (true) {
		/* check if the window is resized */
		/*if (windowIsResized == true) {*/
			/*windowIsResized = false;*/
			/*setBoardWidthHeight(b, COLS, LINES-2);*/
		/*}*/

		if (b->remote == true) {
			/* listen for an INPUT message */
			getNetworkInput(b->snake, b->listenConnection, 
					&GameState);
		} else {
			if (b->ai) {
				/*getAiInput(b->snake, &b->foodSegment);*/
			} else {
				/* get local from a human */
				getLocalInput(b->snake, &GameState);
			}
		}

		switch (GameState) {
		case PLAYING:
			/* update the game */
			update(b);
			/* check if the player has lost, this function
			 * also handles the food */
			if (hasPlayerLost(b) == true) {
				lose(b, &GameState);
			}
			break;
		case QUIT:
			endwin();
			return 0;
		default:
			break;

		}

		if (draw(b, &GameState) != 0) {
			/* there was an error */
			return 1;
		}

		/* wait */
		usleep(SleepingTime);
	}
}
