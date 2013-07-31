#include "snake.h"

void moveSnake(Snake *s, enum Directions d)
{
	/* update previousDirection if neccessary */
	if (s->direction != d) {
		s->previousDirection = s->direction;
		s->direction = d;
	}

	/* move the head */
	switch (d) {
		case LEFT:
			s->head.p.column--;
			break;
		case UP:
			s->head.p.row--;
			break;
		case RIGHT:
			s->head.p.column++;
			break;
		case DOWN:
			s->head.p.row++;
			break;
	}
}

void destroyOldBodySegments(Snake *s)
{
	/* the segments that are not to be removed are put here */
	Stack *tmpStack = newStack();

	/* when to stop iterating */
	int max = s->segmentStack->currPos;

	for (int i=0; i<=max; i++) {
		/* get the current segment */
		Segment *currSeg = pop(s->segmentStack);

		/* first decrease the life */
		currSeg->lifeTicks -= SegmentLife;

		if (currSeg->lifeTicks> 0) {
			/* the segment should live */
			mlog("pushing segment y=%d x=%d", currSeg->p.row, 
					currSeg->p.column);
			push(tmpStack, currSeg);
		} else {
			mlog("freeing segment y=%d x=%d", currSeg->p.row, 
					currSeg->p.column);
			/* the segment should die */
			free(currSeg);
		}
	}

	/* switch stacks, free the old one */
	freeStack(s->segmentStack);
	s->segmentStack = tmpStack;
}

void drawSnake(Snake *s)
{
	/* draw the body */
	loopStack(s->segmentStack, drawSegmentFromVoid);

	/* draw the head */
	drawSnakeHead(s);
}

void drawSnakeHead(Snake *s)
{
	/* draw the head */
	int headChar;
	switch (s->direction) {
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

	mvaddch(s->head.p.row, s->head.p.column, 
			headChar | s->head.color_pair);
}

Snake *newSnake(int y, int x)
{
	Snake *s = (Snake *) malloc(sizeof(Snake));
	s->segmentStack = newStack();
	s->direction = UP;
	s->direction = UP;
	s->segmentLivingTime = 0;
	s->score = 0;

	createHead(&s->head, y, x);

	return s;
}

void freeSnake(Snake *s)
{
	/* free the segments */
	loopStack(s->segmentStack, free);

	/* and the stack */
	freeStack(s->segmentStack);

	/* finally the snake */
	free(s);
}

bool isPositionOccupied(Snake *s, Position p)
{
	for (int i=0; i<s->segmentStack->currPos; i++) {
		Segment *currElement = get(s->segmentStack, i);
		if (positionEqual(p, currElement->p)) {
			return true;
		}
	}

	return false;
}

void addSegmentAtHeadsPosition(Snake *s)
{
	/* set a segment at the heads position */
	Segment *seg = (Segment *) malloc(sizeof(Segment));
	if (seg == NULL) {
		mlog("seg == null");
		return;
	}

	seg->p.row = s->head.p.row;
	seg->p.column = s->head.p.column;
	seg->lifeTicks = SegmentLife + s->segmentLivingTime;
	seg->blocking = true;
	seg->type = BODY;
	seg->color_pair = COLOR_PAIR(BODY_COLOR);
	seg->drawingCharacter = 'o';
	if (s->direction != s->previousDirection) {
		/* the snake is changing direction */
		if (s->previousDirection == RIGHT && s->direction == DOWN) {
			seg->drawingCharacter = ACS_URCORNER;
		} else if (s->previousDirection == RIGHT && s->direction 
				== UP) {
			seg->drawingCharacter = ACS_LRCORNER;
		} else if (s->previousDirection == DOWN && s->direction 
				== RIGHT) {
			seg->drawingCharacter = ACS_LLCORNER;
		} else if (s->previousDirection == DOWN && s->direction
				== LEFT) {
			seg->drawingCharacter = ACS_LRCORNER;
		} else if (s->previousDirection == LEFT && s->direction 
				== UP) {
			seg->drawingCharacter = ACS_LLCORNER;
		} else if (s->previousDirection == LEFT && s->direction 
				== DOWN) {
			seg->drawingCharacter = ACS_ULCORNER;
		} else if (s->previousDirection == UP && s->direction 
				== LEFT) {
			seg->drawingCharacter = ACS_URCORNER;
		} else if (s->previousDirection == UP && s->direction 
				== RIGHT) {
			seg->drawingCharacter = ACS_ULCORNER;
		}
	} else if (s->direction == UP || s->direction == DOWN) {
		seg->drawingCharacter = ACS_VLINE;
	} else if (s->direction == LEFT || s->direction == RIGHT) {
		seg->drawingCharacter = ACS_HLINE;
	}

	push(s->segmentStack, seg);
}

int createHead(Segment *head, int y, int x)
{
	head->p.row = y;
	head->p.column = x;
	head->type = HEAD;
	head->lifeTicks = -1;
	head->blocking = true;
	head->drawingCharacter = 'O';
	head->color_pair = COLOR_PAIR(HEAD_COLOR);
	return 0;
}

void updateSnake(Snake *s)
{
	if (s->segmentStack->currPos >= 0) {
		addSegmentAtHeadsPosition(s);
	}
	moveSnake(s, s->direction);
	destroyOldBodySegments(s);
}
