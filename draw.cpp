#include "draw.hpp"

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

void drawStats(int score)
{
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

void drawTextWindowInMiddle(string text[], int nrows)
{
	/* find the longest string */
	int longestLength = 0;
	for (int i=0; i<nrows; i++) {
		int len = text[i].length();
		if (len  > longestLength) {
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
		mvaddstr(ypos+i+1, xpos+1, text[i].c_str());
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

	string text[8] = {"Press p to pause.", 
		"Use the arrow keys or WASD to move.", 
		"Your head looks like this: ", 
		"Your body: + (don't eat it).", 
		"Food: O (eat it).", 
		"Press q to quit.",
		"Press t to show highscore.",
		"Press h to continue."};
			
	drawTextWindowInMiddle(text, 8);
}

void clearScreen()
{
	if (erase() != OK) {
		endwin();
		throw new FatalException("Could not clear the window.");
	}
}

void refreshScreen()
{
	/*finally refresh the window and wait some time*/
	if (refresh() != OK) {
		endwin();
		throw new FatalException("Could not redraw the window.\n");
	}
}
