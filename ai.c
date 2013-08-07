#include "ai.h"

void init_ai(Board *b)
{
	/*board = b;*/
}

void getNextMove()
{
	struct action *nextAction = TAILQ_LAST(actionList);
	executeAction(nextAction);
}

void executeAction(struct action *a)
{
	setSnakeDirection(board->snake, a->d);
}
