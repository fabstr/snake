#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "snake.h"

int main(int argc, char **argv) 
{
	/* initialization */
	initNCurses();
	Board *b = initGame(COLS, LINES);

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

Board* initGame(int width, int height)
{
	/* the snake board */
	Board *b = (Board *) malloc(sizeof(Board));

	/* set width and height */
	b->width = width;
	b->height = height;

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

	/* get keys as they are pressed */
	cbreak();

	/* don't echo the keys as the are pressed */
	noecho();

	/* prevent flushing on interrupt keys */
	intrflush(stdscr, FALSE);

	/* prevent blocking on getch() */
	nodelay(stdscr, true);
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
			Segment currSeg = b->segments[row][col];
			mvaddch(currSeg.p.row, currSeg.p.column, currSeg.type);
		}
	}

	/* draw the head */
	mvaddch(b->head.p.row, b->head.p.column, b->head.type);
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

void drawBorder(char character) {
	/* draw a border */
	int r, c;

	/* top */
	for (r=0,c=0; c<COLS; c++) { 
		mvaddch(r, c, character);
	}

	/* left */
	for (r=0,c=0; r<LINES; r++) { 
		mvaddch(r, c, character);
	}

	/* bottom */
	for (r=LINES-1,c=0; c<COLS; c++) { 
		mvaddch(r, c, character);
	}

	/* right */
	for (r=0,c=COLS-1; r<LINES; r++) { 
		mvaddch(r, c, character);
	}
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

	if (hp.row < 1 || hp.row >= b->height-1 || hp.column <= 1 ||
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

void updateMovingDirection(Board *b) {
	/* get the new direction */
	switch (getch()) {
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
	}
}

void createBodySegmentAtHeadPosition(Board *b)
{
	/* set asegment at the heads position */
	Segment s;
	s.p.row = b->head.p.row;
	s.p.column = b->head.p.column;
	s.type = BODY;
	s.lifeTicks = SegmentLife;
	s.blocking = true;
	b->segments[b->head.p.row][b->head.p.column] = s;
}

void update(Board *b)
{
	/* get input from user */
	updateMovingDirection(b);

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

/* return non-zero on error */
int draw(Board *b)
{
	/* clear the screen */
	if (erase() != OK) {
		endwin();
		fprintf(stderr, "Could not clear the window.\n");
		return 1;
	}

	/* draw */
	drawBorder(borderCharacter);
	drawSnake(b);

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
		/* update the game */
		update(b);

		if (hasPlayerLost(b) == true) { 
			/* the player has lost */
			/* this function also handles the food */
			return 0;
		}
		
		if (draw(b) != 0) {
			/* there was an error */
			return 1;
		}

		/* wait */
		usleep(SleepingTime);
	}
}
