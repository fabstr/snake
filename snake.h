#include <stdlib.h>
#include <curses.h>

#include "position.h"
#include "stack.h"
#include "segment.h"
#include "colors.h"
#include "mlog.h"

#ifndef SNAKE_H
#define SNAKE_H

/* the time each segment live */
static int SegmentLife = 10;

/* the directions the snake can move */
enum Directions {LEFT, RIGHT, UP, DOWN};


typedef struct Snake {
	/* the head of the snake */
	Segment head;

	/* the body, stack och Segment* */
	Stack *segmentStack;

	/* the latest non-occupied position in the segments array */
	int segPos;

	enum Directions direction;
	enum Directions previousDirection;

	long segmentLivingTime;

	int score;
} Snake;

/**
 * Move the snake one step in its moving direction.
 * The previous direction will be updated if the snake has turned.
 *
 * @param s The snake to move
 * @param d The direction to move in
 */
void moveSnake(Snake *s, enum Directions d);

/**
 * Remove all old body segment from the snake.
 * @param s The snake.
 */
void destroyOldBodySegments(Snake *s);

/**
 * Draw the snake on the screen.
 * @param s The snake to draw.
 */
void drawSnake(Snake *s);

/**
 * Draw the snake's head.
 * @param s The snake.
 */
void drawSnakeHead(Snake *s);

/**
 * Create a new snake.
 * @param y The y coordinate of the head
 * @param x The x coordinate of the head
 * @return The new snake.
 */
Snake* newSnake(int y, int x);

/**
 * Free a snake (from memory).
 * @param s The snake to free
 */
void freeSnake(Snake *s);

/**
 * Return true if the position is occupied by a segment.
 * @param s The snake to check on
 * @param p The position to check
 * @return true if the position is occupied
 */
bool isPositionOccupied(Snake *s, Position p);

/** 
 * Add a segment at the heads position.
 * @param s The snake
 */
void addSegmentAtHeadsPosition(Snake *s);

/**
 * Construct the head at the given y/x coordinates.
 * @param head The head to construct
 * @param y The y coordiate of the head
 * @param x The x coordinate of the head
 * @return 0 on success, else 1.
 */
int createHead(Segment *head, int y, int x);

/** 
 * Update the snake.
 * First a segment will be added, then the snake moved and at last old segments
 * are removed.
 * @param s The snake to move.
 */
void updateSnake(Snake *s);
#endif
