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
	struct Segment *currSeg, *currSegTmp;
	TAILQ_FOREACH_SAFE(currSeg, s->body, segments, currSegTmp) {
		/* first decrease the life */
		currSeg->lifeTicks -= s->segmentLife;

		if (currSeg->lifeTicks <= 0) {
			/* the segment should die */
			TAILQ_REMOVE(s->body, currSeg, segments);
			free(currSeg);
		} 
	}
}

void drawSnake(Snake *s)
{
	/* draw the body */
	struct Segment *currSeg;
	TAILQ_FOREACH(currSeg, s->body, segments) {
		drawSegment(currSeg);
	}

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

	move(s->head.p.row, s->head.p.column);
	addch(headChar | s->head.color_pair);
}

Snake *newSnake(int y, int x)
{
	Snake *s = (Snake *) malloc(sizeof(Snake));
	s->body = (struct snakebody *) malloc(sizeof(struct snakebody));
	TAILQ_INIT(s->body);
	s->direction = UP;
	s->previousDirection = UP;
	s->segmentLivingTime = 0;
	s->score = 0;
	s->segmentLife = 10;

	createHead(&s->head, y, x);

	return s;
}

void freeSnake(Snake *s)
{
	/* free the body */
	struct Segment *currSeg, *currSegTmp;
	TAILQ_FOREACH_SAFE(currSeg, s->body, segments, currSegTmp) {
		TAILQ_REMOVE(s->body, currSeg,  segments);
		free(currSeg);
	}
	free(TAILQ_FIRST(s->body));

	/* and the snake */
	free(s);
}

bool isPositionOccupied(Snake *s, Position p)
{
	struct Segment *currSeg;
	TAILQ_FOREACH(currSeg, s->body, segments) {
		if (positionEqual(p, currSeg->p)) {
			return true;
		}
	}

	return false;
}

void addSegmentAtHeadsPosition(Snake *s)
{
	/* allocate memory */
	struct Segment *seg = (struct Segment *) 
		malloc(sizeof(struct Segment));
	if (seg == NULL) {
		return;
	}

	/* construct the segment */
	seg->p.row = s->head.p.row;
	seg->p.column = s->head.p.column;
	seg->lifeTicks = s->segmentLife + s->segmentLivingTime;
	seg->blocking = true;
	seg->type = BODY;
	seg->color_pair = COLOR_PAIR(BODY_COLOR);

	/* give the segment a correct drawing character (curses corners/lines) 
	 */
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

	/* add the segment */
	TAILQ_INSERT_HEAD(s->body, seg, segments);
}

int createHead(struct Segment *head, int y, int x)
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
	addSegmentAtHeadsPosition(s);
	moveSnake(s, s->direction);
	destroyOldBodySegments(s);
}
