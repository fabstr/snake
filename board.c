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
	/* reset the snake */
	b->snake = resetSnake(b->snake, b->width, b->height);
	/* replace the snake */
	/*if (b->snake != NULL) {*/
		/*freeSnake(b->snake);*/
	/*}*/

	/*b->snake = newSnake(b->height/2, b->width/2);*/

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
		b->snake->segmentLivingTime += b->snake->growthSpeed *
			b->snake->growthSpeed;
		increaseLength(b->snake);
		generateFood(b);
	}
}
