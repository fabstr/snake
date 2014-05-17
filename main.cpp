#include "main.hpp"

int main(int argc, char **argv) 
{
	Options o = parseOptions(argc, argv);

	/* for when the window is resized */
	/*signal(SIGWINCH, resizeBoard);*/
	char *homePath = getenv("HOME");
	char *highscorePath;
	asprintf(&highscorePath, "%s/%s", homePath, highscoreFile.c_str());

	mlog("getting snake color if %s", o.color.argument);
	string *color = new string(o.color.argument);

	mlog("init ncurses");
	initNCurses(color);

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
		o.growthSpeed.argument = (char *) "1";
		o.growthSpeed.set = true;
	}

	if (o.movementSpeed.set != true) {
		o.movementSpeed.argument = (char *) "1";
		o.movementSpeed.set = true;
	}


	mlog("game loop");

	/* run the gaem */
	int toReturn = gameLoop(b);

	string hp = string(highscorePath);
	b->writeHighscore(&hp);


	/* memory deallocation */
	delete b;
	free(highscorePath);

	return toReturn;
}

void initNCurses(string *snakeColor)
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

	} else if (snakeColor->compare("red") == 0) {
		bodycolor = COLOR_RED;
	} else if (snakeColor->compare("green") == 0) {
		bodycolor = COLOR_GREEN;
	} else if (snakeColor->compare("blue") == 0) {
		bodycolor = COLOR_BLUE;
	} else if (snakeColor->compare("white") == 0) {
		bodycolor = COLOR_WHITE;
	} else if (snakeColor->compare("yellow") == 0) {
		bodycolor = COLOR_YELLOW;
	}

	init_color(COLOR_BLUE, 0, 0, 999);
	init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK); /* wall/text */
	init_pair(BODY_COLOR, bodycolor, COLOR_BLACK); /* body */
	init_pair(FOOD_COLOR, COLOR_GREEN, COLOR_BLACK); /* food */
	init_pair(TEXT_INPUT, COLOR_BLACK, COLOR_WHITE); /* text input */
	init_pair(HEAD_COLOR, COLOR_RED, COLOR_BLACK); /* text input */
}

string getTextInput(string msg) 
{
	return getTextInput(&msg);
}

string getTextInput(string *msg)
{
	/* quit ncurses */
	endwin();

	/* show the message */
	cout << msg << endl;
	fflush(stdout);

	/* get the input */
	string s;
	getline(cin, s);

	/* start ncurses again */
	refresh();

	/* return something */
	return s;
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
		string text[3];
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
		string name = getTextInput("What's your name?");

		// construct a record for the player
		Record *r = new Record(b->getSnake()->getScore(), 
				(long) time(NULL),
				&name);
		b->getHighscore()->addRecord(r);
		b->getHighscore()->sort();

		// update the gamestate
		*GameState = HIGHSCORE;
	} else {
		// the player didn't make the top ten
		string text[2];
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

		mlog("drawing board");
		b->draw(&GameState);
		mlog("drawed");

		/* wait */
		usleep(SleepingTime);
	}
}
