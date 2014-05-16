#ifndef SNAKE_H
#define SNAKE_H

#include <stdlib.h>
#include <curses.h>
#include <list>

#include "position.h"
#include "segment.hpp"
#include "colors.h"
#include "mlog.h"
#include "state.h"
#include "protocol.h"


using namespace std;

/* the directions the snake can move */
enum Directions {LEFT, RIGHT, UP, DOWN};

class Snake
{
	/* the snake's head */
	Segment *head;

	/* the body, a list of segments */
	list<Segment> body;

	/* the latest non-occupied position in the segments array */
	int segPos;

	/* the speed the snake grows with */
	int growthSpeed;

	enum Directions direction;
	enum Directions previousDirection;

	/* the time each segment lives */
	long segmentLivingTime;

	int score;

public:
	/**
	 * Move the snake one step in its moving direction.
	 * The previous direction will be updated if the snake has turned.
	 *
	 * @param d The direction to move in
	 */
	void move(enum Directions d);

	/**
	 * Remove all old body segment from the snake.
	 */
	void destroyOldBodySegments();

	/**
	 * Age the snake.
	 * Each segment is aged by growthSpeed.
	 */
	void age();

	/**
	 * Draw the snake on the screen.
	 */
	void draw();

	/**
	 * Draw the snake's head.
	 * @param s The snake.
	 */
	void drawHead();

	/**
	 * Constructor.
	 * segmentLivingTime and growthSpeed will be set to 0 and 1.
	 * @param y The y coordinate of the head
	 * @param x The x coordinate of the head
	 */
	Snake(int y, int x);

	/**
	 * Constructor.
	 * @param y The y coordinate of the head
	 * @param x The x coordinate of the head
	 * @param segmentLivingTime The time each segment lives
	 * @param growthSpeed The amount of segments the snake growths with
	 */
	Snake(int y, int x, int segmentLivingTime, int growthSpeed);

	/**
	 * Create a new snake.
	 * @param y The y coordinate of the head
	 * @param x The x coordinate of the head
	 * @return The new snake.
	 */
	Snake* newSnake(int y, int x);

	/**
	 * Reset the snake to its head, positioned at (x, y).
	 * @param y The y position of the new snake
	 * @param x The x position of the new snake
	 */
	void reset(int y, int x);

	/**
	 * Destructor.
	 */
	~Snake();

	/**
	 * Return true if the position is occupied by a segment.
	 * @param p The position to check
	 * @return true if the position is occupied
	 */
	bool isPositionOccupied(Position p);

	/** 
	 * Add a segment at the heads position.
	 */
	void addSegmentAtHeadsPosition();

	/**
	 * Construct the head at the given y/x coordinates.
	 * @param head The head to construct
	 * @param y The y coordiate of the head
	 * @param x The x coordinate of the head
	 * @return 0 on success, else 1.
	 */
	void createHead(int y, int x);

	/** 
	 * Update the snake.
	 * First a segment will be added, then the snake moved and at last old 
	 * segments are removed.
	 */
	void update();

	/**
	 * Set the snake's direction. 
	 * It will be checked the snake isn't moved backwards, (cant go left if 
	 * going right etc)
	 * @param d The direction to set
	 */
	void setDirection(enum Directions d);

	/**
	 * Update the moving direction, according to the result of getch().
	 * @param GameState The current game state
	 */
	void getLocalInput(State *GameState);

	/**
	 * Like getLocalInput, but listen for INPUT messages on the given 
	 * connection.
	 * @param c The connection to listen on
	 * @param GameState The current game state
	 */
	void getNetworkInput(Connection *c, State *GameState);

	/**
	 * Increase the snake's length by 1.
	 */
	void increaseLength();

	/**
	 * Get the snake's head.
	 * @return The head.
	 */
	Segment *getHead();

	/**
	 * Increase the snake's score.
	 * @param amount
	 */
	void increaseScore(int amount);

	/**
	 * Increase the segment living time.
	 * @param amount
	 */
	void increaseSegmentLivingTime(int amount);

	/**
	 * Get the growth speed.
	 * @return The growth speed.
	 */
	int getGrowthSpeed();

	/**
	 * Get the score.
	 * @return The score.
	 */
	int getScore();

	/**
	 * Log the body to snake.log
	 */
	void logBody();

	/**
	 * Eat food.
	 * Increase the score, the living time and the length.
	 */
	void eatFood();
};

#endif
