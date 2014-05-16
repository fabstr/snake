#include "main.hpp"

int main(int argc, char **argv) 
{
	Options o = parseOptions(argc, argv);

	/* for when the window is resized */
	/*signal(SIGWINCH, resizeBoard);*/
	char *homePath = getenv("HOME");
	char *highscorePath;
	asprintf(&highscorePath, "%s/%s", homePath, HIGHSCORE_FILE);

	mlog("init ncurses");
	initNCurses(o.color.argument);

	mlog("checking width");
	if (COLS < 41 || LINES < 6) {
		endwin();
		fprintf(stderr, "The terminal window needs to be at least 41");
		fprintf(stderr, " chars wide and 6 lines high.\n");
		exit(EXIT_FAILURE);
	} 

	mlog("reading cmdlineargs");
	int growthSpeed = 1; //atoi(o.growthSpeed.argument);
	int movementSpeed = 1; //atoi(o.movementSpeed.argument);

	// create the board for the correct window size and highscore file
	string path = string(highscorePath);
	mlog("new board");
	Board *b = new Board(COLS, LINES, &path, growthSpeed, movementSpeed);

	if (b == NULL) {
		endwin();
		fprintf(stderr, "Could not initialize the board.\n");
		exit(EXIT_FAILURE);
	}

	if (o.remote.set == true) {
		mlog("opening connection at %s", o.port.argument);
		Connection *c = openListenConnection(o.port.argument);
		if (c == NULL) {
			fprintf(stderr, "Could not open socket to listen: %s",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		b->setListenConnection(c);
		b->setRemote(true);
		mlog("connection opened");
	} else {
		if (o.ai.set == true) {
			mlog("ai steering");
			b->setAi(true);
			/*init_ai(b->width, b->height);*/
		} else {
			mlog("local human steering");
			b->setAi(false);
			b->setListenConnection(NULL);
			b->setRemote(false);
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


	mlog("game loop");

	/* run the gaem */
	int toReturn = gameLoop(b);

	b->writeHighscore(highscorePath);


	/* memory deallocation */
	delete b;
	free(highscorePath);

	return toReturn;
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
	mlog("score: %d", b->getSnake()->getScore());

	// get the highscore table and the record with the lowest score
	HighscoreTable *ht = b->getHighscore();
	Record *lowest = ht->getLowest();

	// get the player's score
	int playerScore = b->getSnake()->getScore();

	if (playerScore > lowest->getScore()) {
		// print a dialog box 
		char **text = (char **) malloc(3*sizeof(char **));
		text[0] = "                    YOU LOST!                 ";
		text[1] = "Please enter your name for the highscore list.";
		text[2] = "           Press space to continue.           ";
		drawTextWindowInMiddle(text, 3);
		refresh();
		free(text);

		// wait for the user to press space
		char c;
		while ((c = getch()) != ' ') {
			usleep(10000);
		}

		// get the user's name 
		char *name = (char *) malloc(64);
		if (getTextInput("What's your name? ", name, 64) != 0) {
			*GameState = QUIT;
		}

		// construct a record for the player
		Record *r = new Record(b->getSnake()->getScore(), 
				(long) time(NULL),
				name);
		b->getHighscore()->addRecord(r);
		b->getHighscore()->sort();

		// update the gamestate
		*GameState = HIGHSCORE;
	} else {
		// the player didn't make the top ten
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

	b->resetGame();
}

int gameLoop(Board *b) 
{
	mlog("starting game loop");
	State GameState = PLAYING;
	while (true) {
		/* check if the window is resized */
		/*if (windowIsResized == true) {*/
			/*windowIsResized = false;*/
			/*setBoardWidthHeight(b, COLS, LINES-2);*/
		/*}*/

		if (b->getRemote()== true) {
			/* listen for an INPUT message */
			b->getSnake()->getNetworkInput(b->getListenConnection(),
					&GameState);
		} else {
			if (b->getAi()) {
				/*getAiInput(b->snake, &b->foodSegment);*/
			} else {
				/* get local from a human */
				b->getSnake()->getLocalInput(&GameState);
			}
		}

		switch (GameState) {
		case PLAYING:

			/* check if the player has lost, this function
			 * also handles the food */
			if (b->hasPlayerLost() == true) {
				mlog("player has lost");
				lose(b, &GameState);
			}

			/* update the game */
			mlog("updating game");
			b->update();

			break;
		case QUIT:
			endwin();
			return 0;
		default:
			break;

		}

		b->draw(&GameState);

		/* wait */
		usleep(SleepingTime);
	}
}
