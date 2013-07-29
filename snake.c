#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "snake.h"

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

	/* memory deallocation */
	for (int row=0; row<b->height; row++) {
		if (b->segments[row] != NULL) {
			free(b->segments[row]);
		}
	}
	free(b->segments);
	free(b);

	/* pass through the exit status */
	return toReturn;
}

HighscoreEntry HighscoreEntryFromString(char *str)
{
	HighscoreEntry he;

	/* split the string */
	char *tmparr[2];
	char *current;
	char sep = ' ';
	int i;
	while ((current = strsep(&str, &sep)) != NULL) {
		tmparr[i++] = current;
	}

	he.score = atoi(tmparr[0]);
	he.name = (char *) malloc(strlen(tmparr[1])+1);
	memcpy(he.name, tmparr[1], strlen(tmparr[1])+1);

	return he;
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

	/* start moving upwards */
	b->direction = UP;

	/* set a starting position */
	b->head.p.row = height/2;
	b->head.p.column = width/2;

	/* initialize the segments array */
	b->segments = (Segment **) malloc(b->height*sizeof(Segment*));

	for (int i=0; i<b->height; i++) {
		b->segments[i] = (Segment *) malloc(b->width*sizeof(Segment));
	}

	for (int row=0; row<b->height; row++) {
		for (int col=0; col<b->width; col++) {
			struct timeval t;
			gettimeofday(&t, NULL);
			Segment s = {{row, col}, AIR, -1, false};
			b->segments[row][col] = s;
		}
	}

	/* we start with 0 food */
	b->food = 0;

	/* add some food */
	generateFood(b);

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

	/* initialize the colours */
	init_color(COLOR_BLUE, 0, 0, 999);
	init_pair(TEXT_COLOR_INDEX, COLOR_WHITE, COLOR_BLACK); /* wall/text */
	init_pair(BODY_COLOR_INDEX, COLOR_BLUE, COLOR_BLACK); /* body */
	init_pair(FOOD_COLOR_INDEX, COLOR_GREEN, COLOR_BLACK); /* food */
}

void destroyOldBodySegments(Board *b)
{
	for (int row=0; row<b->height; row++) {
		for (int col=0; col<b->width; col++) {
			/*check if the segment should be removed*/
			if (b->segments[row][col].type == BODY) {

				/* first decrease the life ticks */
				b->segments[row][col].lifeTicks -= 10;

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

void drawBorder(int col1, int row1, int col2, int row2) {
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

void lose(Board *b) 
{
	sleep(1);
	endwin();
	printf("You lost with %d %s.\n", b->food, 
			(b->food == 1) ? "point" : "points");
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
			lose(b);
			return true;
		}
	}

	if (headIsOutOfBoard(b) == true) {
		lose(b);
		return true;
	}

	return false;
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
}

void drawTextWindow(char **text, int ncols, int nrows, int xpos, int ypos)
{
	/* draw the border */
	drawBorder(xpos, ypos, xpos+ncols+1, ypos+nrows+1);

	/* draw the text */
	for (int i=0; i<nrows; i++) {
		/* ypos+0 and xpos+0 is the border */
		mvaddstr(ypos+i+1, xpos+1, text[i]);
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

	char *text[7] = {"Press p to pause.", 
		"Use the arrow keys or WASD to move.", 
		"Your head looks like this: ", 
		"Your body: + (don't eat it).", 
		"Food: O (eat it).", 
		"Press q to quit.",
		"Press h to continue."};
			
	drawTextWindow(text, longestStringLength, 7, x, y-7);
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
		case PAUSED:
			textarr[0] = "The game is paused.";
			textarr[1] = "Press p to continue.";
			drawTextWindow(textarr, strlen(textarr[1]), 2, 
					(COLS-strlen(textarr[1])-1)/2, LINES/2-3);
			break;
		default:
			break;
	}

	/* the cursor is ugly, move it to bottom-right */
	move(LINES-1, COLS-1);

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
					return 0;
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
