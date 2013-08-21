#ifndef AI_H
#define AI_H

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "snake.h"
#include "segment.h"
#include "astar.h"

enum TurnPositions {
	TOPLEFT=0, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, LWALL, RWALL, TWALL,
	BWALL
};

typedef struct action action;
struct action {
	enum Directions d;
	/**< @brief The direction to move */

	action *next;
	/**< @brief The next element in the linked list */
};

void init_ai(int boardWidth, int boardHeight);
void getAiInput(Snake *s, struct Segment *food);
void free_ai();
void getNewPath(Snake *s, struct Segment *food);
enum Directions getDirection(Node *child, Node *parent);

#endif
