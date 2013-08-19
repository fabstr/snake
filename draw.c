#include "draw.h"

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
	int ypos = LINES/2 - nrows/2 - 2;

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
		asprintf(&text[i+1], "%2d. %4d %s", i+1, currRec.score, 
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
int draw(Board *b, State *GameState)
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
	switch (*GameState) {
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
	default:
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
