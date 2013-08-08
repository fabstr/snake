#ifndef AI_H
#define AI_H

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "snake.h"
#include "segment.h"

enum TurnPositions {
	TOPLEFT=0, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, LWALL, RWALL, TWALL, BWALL
};

static char *enumStrings[] = {
	 "TOPLEFT", "TOPRIGHT", "BOTTOMLEFT", "BOTTOMRIGHT", "LWALL", "RWALL", "TWALL", "BWALL"
};

struct action {
	enum Directions d;
	/**< @brief The direction to move */

	Position p;
	/**< @brief Activate the action when the snake's head's here. */

	SLIST_ENTRY(action) actions;
	/**< @brief The next element in the linked list */
};

void init_ai(int boardWidth, int boardHeight);
void getAiInput(Snake *s, struct Segment *food, int boardWidth, 
		int boardHeight);
void free_ai();
bool atTurnPosition(Position p, int width, int height);
enum TurnPositions getTurnPosition(Position p, int width, int height);

int **board;
int width; 
int height;

#endif
