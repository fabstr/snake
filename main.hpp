#ifndef MAIN_H
#define MAIN_H

#define _WITH_GETLINE
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>

#include "highscore.hpp"
#include "snake.hpp"
#include "position.h"
#include "segment.hpp"
#include "colors.h"
#include "draw.hpp"
#include "board.hpp"
#include "cmdlineargs.h"

using namespace std;

/* the time the loop sleeps between the iterations, micro seconds */
static int SleepingTime = 100 * 1000;
//static int SleepingTime = 1;

/* set to true when the window is resized */
/*static bool windowIsResized = false;*/

string highscoreFile = ".snakehighscore";

/**
 * Initialize the board.
 * @param width The width of the playing board (in columns).
 * @param height The height of the playing board (in rows).
 * @param highscorePath The absolute path to the highscore file.
 * @return A Board * to the initialized board.
 */
Board *initGame(int width, int height, string *highscorePath);

/**
 * Perform inialization for ncurses.
 * @param snakeColor The color of the snake. Can be "red", "green", "blue" or 
 *                   "white".
 */
void initNCurses(string *snakeColor);

/**
 * Get text input from the user.
 * @param msg A meessage to print before prompting.
 * @param dest A buffer of size bufflen to hold the input.
 * @param bufflen The length of the dest buffer.
 * @return The string.
 */
string getTextInput(string msg);
string getTextInput(string *msg);

/**
 * Called uppon reciving SIGWINCH.
 */
/*void resizeBoard(int i);*/

/**
 * Set the board's width and height.
 * @param b The board to set on.
 * @param windowCols The width
 * @param windowRows The height
 */
void setBoardWidthHeight(Board *b, int windowCols, int windowRows);

/**
 * Quit ncurses, print a message with the score and return 0.
 * @param b The board used for the game.
 * @param GameState A pointer to the current gamestate
 */
void lose(Board *b, State *GameState);

/**
 * The main game loop.
 * @param b The board of the current game.
 */
int gameLoop(Board *b);

#endif
