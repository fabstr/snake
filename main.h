#define _WITH_GETLINE
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "highscore.h"
#include "snake.h"
#include "position.h"
#include "segment.h"
#include "colors.h"

#ifndef MAIN_H
#define MAIN_H

/* the time the loop sleeps between the iterations, micro seconds */
static int SleepingTime = 100 * 1000;

/* set to true when the window is resized */
/*static bool windowIsResized = false;*/

char* highscorePath = "snakehighscore";

enum State {PLAYING, PAUSED, HELP, QUIT, HIGHSCORE};
/* true when the game is paused */
static enum State GameState = PLAYING;

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
	Segment foodSegment;

	HighscoreTable *highscore;
} Board;

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
 * Checks if the position is occupied on the board. 
 * If the segment is non-air, it is.
 * @param p The position to check.
 * @param b The board to check on.
 * @return true if the position is occupied.
 */
bool positionIsOccupied(Position p, Board *b);

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
 * The main game loop.
 * @param b The board of the current game.
 */
int gameLoop(Board *b);

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
/*void resizeBoard(int i);*/
void setBoardWidthHeight(Board *b, int windowCols, int windowRows);

#endif
