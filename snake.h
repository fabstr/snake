#include <stdlib.h>
#include <curses.h>

#include "position.h"
#include "segment.h"
#include "colors.h"
#include "mlog.h"
#include "queue.h"
#include "state.h"
#include "protocol.h"

#ifndef SNAKE_H
#define SNAKE_H

/* the directions the snake can move */
enum Directions {LEFT, RIGHT, UP, DOWN};

typedef struct {
	/* the snake's head */
	struct Segment head;

	/* the body, a tail queue of segments */
	TAILQ_HEAD(snakebody, Segment);
	struct snakebody *body;

	/* the latest non-occupied position in the segments array */
	int segPos;

	/* the speed the snake grows with */
	int growthSpeed;

	enum Directions direction;
	enum Directions previousDirection;

	/* the time each segment lives */
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
int createHead(struct Segment *head, int y, int x);

/** 
 * Update the snake.
 * First a segment will be added, then the snake moved and at last old segments
 * are removed.
 * @param s The snake to move.
 */
void updateSnake(Snake *s);

/**
 * Set the snake's direction. 
 * It will be checked the snake isn't moved backwards, (cant go left if going
 * right etc)
 * @param s The snake
 * @param d The direction to set
 */
void setSnakeDirection(Snake *s, enum Directions d);

/**
 * Update the moving direction, according to the result of getch().
 * @param s The snake to update
 * @param GameState The current game state
 */
void getLocalInput(Snake *s, State *GameState);

/**
 * Like getLocalInput, but listen for INPUT messages on the given connection.
 * @param s The snake to update
 * @param c The connection to listen on
 */
void getNetworkInput(Snake *s, Connection *c, State *GameState);

void increaseLength(Snake *s);
#endif
