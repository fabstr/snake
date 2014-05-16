#ifndef BOARD_H
#define BOARD_H

#include "segment.hpp"
#include "snake.hpp"
#include "state.h"
#include "protocol.h"
#include "FatalException.hpp"
#include "draw.hpp"
#include "highscore.hpp"
#include "mlog.h"


/**
 * The playing board keeps track of the snake, the segments and the food 
 * (score). It also has width and height.
 */
class Board {
	/* the size of the board */
	int width;
	int height;

	/* the player */
	Snake *snake;

	/* the food */
	Segment *foodSegment;

	HighscoreTable *highscore;

	bool remote;
	
	Connection *listenConnection;

	bool ai;
public:
	/**< @brief if true, run ai instead of input from a human */
	/**
	 * Generate food.
	 * amount segments of type FOOD is placed on a random non-occupied 
	 * position on the board.
	 * @param b The board to place food on
	 * @param amount The number of pieces to generate
	 */
	void generateFood();

	/**
	 * Checks if the head is out of bounds, ie hits the border.
	 * @param b The board to check on.
	 * @return True if the head hits the border, else false.
	 */
	bool headIsOutOfBoard();

	/**
	 * Update the game board.
	 * @param b The board
	 */
	void update();

	/**
	 * Check if the playr has lost.
	 * @param b The board to check on
	 * @return True if the player has lost.
	 */
	bool hasPlayerLost();

	/**
	 * Reset the game.
	 * @param b The board to reset
	 */
	void resetGame();

	/**
	 * Check for food.
	 * If the head is at a food segment, increase the score and generate new 
	 * food.
	 * @param b The board to check on
	 */
	void checkForFood();

	/**
	 * Add a snake. 
	 * @param s The snake.
	 */
	void addSnake(Snake *s);

	/**
	 * Get the width of the board.
	 * @return The width.
	 */
	int getWidth();

	/**
	 * Get the height of the board.
	 * @return The height.
	 */
	int getHeight();

	/**
	 * Constructor.
	 * @param w The width of the board
	 * @param h The height of the board
	 * @param highscorePath The path to the highscore table
	 * @param growthSpeed The growth speed of the snake
	 * @param movementSpeed The movement speed of the snake
	 */
	Board(int w, int h, string *highscorePath, int growthSpeed, 
			int movementSpeed);

	/**
	 * Destructor.
	 * delete's the highscore table.
	 */
	~Board();

	/**
	 * Set the listen connection.
	 * @param c The connection.
	 */
	void setListenConnection(Connection *c);

	/**
	 * Set remote.
	 * @param value The value
	 */
	void setRemote(bool value);


	/**
	 * Set ai.
	 * @param value The value.
	 */
	void setAi(bool value);

	/**
	 * Get the snake.
	 * @return The snake.
	 */
	Snake *getSnake();

	/**
	 * Write the highscore file to path.
	 * @param path The highscore file.
	 */
	void writeHighscore(char *path);

	/**
	 * Get the highscore table.
	 * @return The highscore table.
	 */
	HighscoreTable *getHighscore();

	/**
	 * Get the value of remote.
	 * @return remote
	 */
	bool getRemote();

	/**
	 * Get the value of ai.
	 * @return ai
	 */
	bool getAi();

	/**
	 * Get the listen connection.
	 * @return listenConnection
	 */
	Connection *getListenConnection();

	/**
	 * Draw the food and the snake.
	 */
	void draw(State *GameState);
};

#endif
