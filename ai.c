#include "ai.h"

void init_ai(int boardWidth, int boardHeight)
{
}

void snakeToBoard(Snake *s, int *board, int width)
{
	struct Segment *currSeg;
	TAILQ_FOREACH(currSeg, s->body, segments) {
		int row = currSeg->p.row;
		int column = currSeg->p.column;
		board[row*width + column] = currSeg->lifeTicks;
	}
}

bool atTurnPosition(Position p, int width, int height)
{
	if (p.row == 2 || p.row == height-3 || p.column == 2 ||
			p.column == width-3) {
		return true;
	}
	
	return false;
}

enum TurnPositions getTurnPosition(Position p, int width, int height)
{
	if (p.row == 1) {
		if (p.column == 2) {
			return TOPLEFT;
		} else if (p.column == width - 3) {
			return TOPRIGHT;
		} else {
			return TWALL;
		}
	} else if (p.row == height - 2) {
		if (p.column == 2) {
			return BOTTOMLEFT;
		} else if (p.column == width - 3) {
			return BOTTOMRIGHT;
		} else {
			return BWALL;
		}
	} else if (p.column == 2) {
		return LWALL;
	} else {
		return RWALL;
	}
}

void getAiInput(Snake *s, struct Segment *food, int boardWidth, int boardHeight)
{
	/*static SLIST_HEAD(slisthead, action) movestack */
		/*= SLIST_HEAD_INITIALIZER(movestack);*/

	static int width = -1;
	static int height = -1;
	if (width == -1) {
		width = boardWidth;
	}
	if (height == -1) {
		height = boardHeight;
	}

	static int *board = NULL;
	if (board == NULL) {
		board = (int *) malloc(sizeof(int)*width*height);
		if (board == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
	}

	memset(board, 0, sizeof(int)*width*height);

	snakeToBoard(s, board, width);

	if (atTurnPosition(s->head.p, width, height) == true) {
		mlog("turn position: %s at row=%d col=%d", enumStrings[getTurnPosition(s->head.p, width, height)], s->head.p.row, s->head.p.column);
		switch (getTurnPosition(s->head.p, width, height)) {
			case TOPLEFT:
				if (s->direction == LEFT) {
					setSnakeDirection(s, DOWN);
				} else if (s->direction == UP) {
					setSnakeDirection(s, RIGHT);
				}
				break;
			case TOPRIGHT:
				if (s->direction == RIGHT) {
					setSnakeDirection(s, DOWN);
				} else if (s->direction == UP) {
					setSnakeDirection(s, LEFT);
				}
				break;
			case BOTTOMLEFT:
				if (s->direction == LEFT) {
					setSnakeDirection(s, UP);
				} else if (s->direction == DOWN) {
					setSnakeDirection(s, RIGHT);
				}
				break;
			case BOTTOMRIGHT:
				if (s->direction == RIGHT) {
					setSnakeDirection(s, UP);
				} else if (s->direction == DOWN) {
					setSnakeDirection(s, LEFT);
				}
				break;
			case LWALL:
				setSnakeDirection(s, RIGHT);
				break;
			case RWALL:
				setSnakeDirection(s, LEFT);
				break;
			case TWALL:
				setSnakeDirection(s, DOWN);
				break;
			case BWALL:
				setSnakeDirection(s, UP);
				break;
		}
	} else if (food->p.row < s->head.p.row) {
		/* we're under the food row */
		switch (s->direction) {
			case LEFT:
			case RIGHT:
				setSnakeDirection(s, UP);
				break;
			case DOWN:
				setSnakeDirection(s, LEFT);
				break;
			case UP:
				break;
		}
	} else if (food->p.row > s->head.p.row) {
		/* we're above the food row */
		switch (s->direction) {
			case LEFT:
			case RIGHT:
				setSnakeDirection(s, DOWN);
				break;
			case UP:
				setSnakeDirection(s, LEFT);
				break;
			case DOWN:
				break;
		}
	} else if (food->p.column < s->head.p.column) {
		/* we're right of the food segment */
		switch (s->direction) {
			case UP:
			case DOWN:
				setSnakeDirection(s, LEFT);
				break;
			case RIGHT:
				setSnakeDirection(s, UP);
				break;
			case LEFT:
				break;
		}
	} else {
		/* we're left of the food segment */
		switch (s->direction) {
			case UP:
			case DOWN:
				setSnakeDirection(s, RIGHT);
				break;
			case LEFT:
				setSnakeDirection(s, UP);
				break;
			case RIGHT:
				break;
		}
	}
}

void free_ai()
{
	for (int i=0; i<height; i++) {
		free(board[i]);
	}
	free(board);
}
