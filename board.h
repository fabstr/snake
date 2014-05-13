#include "segment.h"
#include "highscore.h"
#include "snake.h"
#include "state.h"
#include "protocol.h"

#ifndef BOARD_H
#define BOARD_H

/**
 * The playing board keeps track of the snake, the segments and the food 
 * (score). It also has width and height.
 */
typedef struct Board {
	/* the size of the board */
	int width;
	int height;

	/* the player */
	Snake *snake;

	/* the food */
	struct Segment foodSegment;

	HighscoreTable *highscore;

	bool remote;
	
	Connection *listenConnection;

	bool ai;
	/**< @brief if true, run ai instead of input from a human */
} Board;

/**
 * Generate food.
 * amount segments of type FOOD is placed on a random non-occupied position on the 
 * board.
 * @param b The board to place food on
 * @param amount The number of pieces to generate
 */
void generateFood(Board *b);

/**
 * Checks if the head is out of bounds, ie hits the border.
 * @param b The board to check on.
 * @return True if the head hits the border, else false.
 */
bool headIsOutOfBoard(Board *b);

/**
 * Update the game board.
 * @param b The board
 */
void update(Board *b);

/**
 * Check if the playr has lost.
 * @param b The board to check on
 * @return True if the player has lost.
 */
bool hasPlayerLost(Board *b);

/**
 * Reset the game.
 * @param b The board to reset
 */
void resetGame(Board *b);

/**
 * Check for food.
 * If the head is at a food segment, increase the score and generate new food.
 * @param b The board to check on
 */
void checkForFood(Board *b);
#endif
