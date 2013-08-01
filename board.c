#include "board.h"

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

void getInput(Board *b, State *GameState) {
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
			*GameState = (*GameState == PAUSED) ? PLAYING : PAUSED;
			break;
		case 'h':
			*GameState = (*GameState == HELP) ? PLAYING : HELP;
			break;
		case 'q':
			*GameState = QUIT;
			break;
		case 't':
			*GameState = (*GameState == HIGHSCORE) ?
				PLAYING : HIGHSCORE;
			break;
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

void checkForFood(Board *b)
{
	if (positionEqual(b->foodSegment.p, b->snake->head.p) == true) {
		/* there is food here! 
		 *  - Increase the score
		 *  - Increase the living time (the length)
		 *  - add a segment at the head
		 *  - create more food*/
		b->snake->score++;
		b->snake->segmentLivingTime += b->snake->segmentLife;
		addSegmentAtHeadsPosition(b->snake);
		generateFood(b);
	}
}
