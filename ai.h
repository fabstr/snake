#ifndef AI_H
#define AI_H

#include "board.h"
#include "snake.h"

struct action {
	enum Directions d;
	/**< The direction to move when this action is executed */

	TAILQ_ENTRY(action) nextAction;
	/**< The next action to execute */
};

//static Board *board;

struct action *actionList;

void init_ai(Board *b);
void executeAction(struct action *a);
void getNextMove();

#endif
