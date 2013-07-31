#include <stdbool.h>

#ifndef POSITION_H
#define POSITION_H

/**
 * A position with column/row coordinates.
 */
typedef struct Position {
	/* the coordinates */
	int column;
	int row;
} Position;

bool positionEqual(Position p1, Position p2);

#endif
