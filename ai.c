#include "ai.h"

int boardWidth;
int boardHeight;
action *nextAction;

void init_ai(int bw, int bh)
{
	boardWidth = bw;
	boardHeight = bh;
	nextAction = NULL;
}

void getAiInput(Snake *s, struct Segment *food)
{
	mlog("getting ai input s at row=%d col=%d", s->head.p.row,
			s->head.p.column);
	if (nextAction == NULL) {
		getNewPath(s, food);
	} else {
		mlog("direction:");
		switch (nextAction->d) {
			case LEFT:
				mlog("left");
				break;
			case RIGHT:
				mlog("right");
				break;
			case UP:
				mlog("up");
				break;
			case DOWN:
				mlog("down");
				break;
		}
		setSnakeDirection(s, nextAction->d);
		action *toFree = nextAction;
		nextAction = nextAction->next;
		free(toFree);
	}
}

void free_ai()
{
}

void getNewPath(Snake *s, struct Segment *food)
{
	Node nodes[boardWidth*boardHeight];

	for (int x=0; x<boardWidth; x++) {
		for (int y=0; y<boardHeight; y++) {
			Node *currNode = getNodeAt(x, y, nodes, boardHeight);
			currNode->penetrable = true;
		}
	}

	/* mark the snake's body as inpenetrable */
	struct Segment *currSeg;
	TAILQ_FOREACH(currSeg, s->body, segments) {
		int x = currSeg->p.column;
		int y = currSeg->p.row;

		Node *currNode = getNodeAt(x, y, nodes, boardHeight);
		currNode->penetrable = false;
	}

	mlog("getPath(...)");

	Node *goal = getPath(s->head.p.column, s->head.p.row, food->p.column, food->p.row, nodes, boardWidth, boardHeight);
	if (goal == NULL) {
		mlog("goal was NULL");
		/* there was no path */
		return;
	}

	mlog("goal gotten");

	Node *child = goal;
	while (child != NULL) {
		action *newAction = (action *) malloc(sizeof(action));
		newAction->next = nextAction;

		Node *parent = child->parent;

		newAction->d = getDirection(child, parent);
		nextAction = newAction;

		child = child->parent;
	}
	
	return;
}

enum Directions getDirection(Node *child, Node *parent)
{
	if (parent->x > child->x) {
		/* parent is right */
		return RIGHT;
	} else if (parent->x < child->x) {
		/* parent is left */
		return LEFT;
	} else if (parent->y < child->y) {
		/* parent is above */
		return UP;
	} else {
		return DOWN;
	}
}
