#define _WITH_GETLINE
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "highscore.h"

#ifndef SNAKE_H
#define SNAKE_H

#define TEXT_COLOR_INDEX 1
#define BODY_COLOR_INDEX 2
#define FOOD_COLOR_INDEX 3
#define TEXT_INPUT_INDEX 4

/* the time the loop sleeps between the iterations, micro seconds */
static int SleepingTime = 100 * 1000;

/* the number of segments the player starts with */
static const int StartingSegmentsCount = 0;

/* the time each segment live */
static int SegmentLife = 10;

/* how many body parts to add times 10 (ie add 1 body part for each food) */
static int LifeTicksDecreaseSpeed = 10;

char* highscorePath = "snakehighscore";

enum State {PLAYING, PAUSED, HELP, QUIT, HIGHSCORE};
/* true when the game is paused */
static enum State GameState = PLAYING;

/* the types a segment can have, and the character the type is rendered as */
enum SegmentType {HEAD, BODY, WALL, AIR, FOOD};

/* the directions the snake can move */
enum Directions {LEFT, RIGHT, UP, DOWN};

/**
 * A position with column/row coordinates.
 */
typedef struct Position {
	/* the coordinates */
	int column;
	int row;
} Position;

/**
 * A segment is placed on the playing board.
 */
typedef struct Segment {
	/* the position of the segment */
	Position p;

	/* the type of the segment (ie corner, body, wall) */
	enum SegmentType type;

	/* when 0, the segment will be altered to AIR */
	int lifeTicks;

	/* if true, the player will lose if hitting this segment */
	bool blocking;

	int drawingCharacter;
} Segment;


/**
 * The playing board keeps track of the snake, the segments and the food 
 * (score). It also has width and height.
 */
typedef struct Board {
	/* the size of the board */
	int width;
	int height;

	/* the snake's body */
	Segment **segments;
	
	/* the snake's head */
	Segment head;

	/* the direction the snake is moving */
	enum Directions direction;
	enum Directions previousDirection;

	/* the time each segment lives, in micro seconds */
	long segmentLivingTime;

	/* the player's score */
	int food;

	HighscoreTable *highscore;
} Board;

/**
 * A logging function.
 * @param msg The string to log.
 */
void mlog(char *msg);

/**
 * Initialize the board.
 * @param width The width of the playing board (in columns).
 * @param height The height of the playing board (in rows).
 * @return A Board * to the initialized board.
 */
Board *initGame(int width, int height);

/**
 * Perform inialization for ncurses.
 */
void initNCurses();

/**
 * Draw the snake at its current position.
 * @param b The board the snake is on.
 */
void drawSnake(Board *b);

/**
 * Checks if the position is occupied on the board. 
 * If the segment is non-air, it is.
 * @param p The position to check.
 * @param b The board to check on.
 * @return true if the position is occupied.
 */
bool positionIsOccupied(Position p, Board *b);

/**
 * Destroy old body aegments.
 * @param b The board to destroy from.
 */
void destroyOldBodySegments(Board *b);

/**
 * Draw a border.
 * @param character The character to draw the border with.
 */
void drawBorder(int col1, int row1, int col2, int row2);

/**
 * Generate food.
 * amount segments of type FOOD is placed on a random non-occupied position on the 
 * board.
 * @param b The board to place food on
 * @param amount The number of pieces to generate
 */
void generateFood(Board *b);

/**
 * Quit ncurses, print a message with the score and return 0.
 * @param b The board used for the game.
 */
void lose(Board *b);

/**
 * Checks if the head is out of bounds, ie hits the border.
 * @param b The board to check on.
 * @return True if the head hits the border, else false.
 */
bool headIsOutOfBoard(Board *b);

/**
 * Move the snake head in its direction.
 * @param b The board to move the snake on.
 */
void moveSnakeHead(Board *b);

/**
 * Update the moving direction, according to the result of getch().
 * @param b The board to update on.
 */
void getInput(Board *b);

/**
 * Place a body segment at the head's position.
 * @param b The current playing board.
 */
void createBodySegmentAtHeadPosition(Board *b);

/**
 * The main game loop.
 * @param b The board of the current game.
 */
int gameLoop(Board *b);

void destroyOldBodySegments(Board *b);
void drawBorder(int col1, int row1, int col2, int row2);
void update(Board *b);
bool hasPlayerLost(Board *b);
void drawStats(Board *b);
void drawTextWindowInMiddle(char **text, int nrows);
void drawHighscore(Board *b);
void drawHelp();
int draw(Board *b);
void resetGame(Board *b);
int getTextInput(char *msg, char *dest, size_t bufflen);

#endif