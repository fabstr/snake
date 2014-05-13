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
#include "draw.h"
#include "board.h"
#include "cmdlineargs.h"

#ifndef MAIN_H
#define MAIN_H

/* the time the loop sleeps between the iterations, micro seconds */
static int SleepingTime = 100 * 1000;
//static int SleepingTime = 1;

/* set to true when the window is resized */
/*static bool windowIsResized = false;*/

char* HIGHSCORE_FILE = ".snakehighscore";

/**
 * Initialize the board.
 * @param width The width of the playing board (in columns).
 * @param height The height of the playing board (in rows).
 * @param highscorePath The absolute path to the highscore file.
 * @return A Board * to the initialized board.
 */
Board *initGame(int width, int height, char *highscorePath);

/**
 * Perform inialization for ncurses.
 */
void initNCurses();

/**
 * Get text input from the user.
 * @param msg A meessage to print before prompting.
 * @param dest A buffer of size bufflen to hold the input.
 * @param bufflen The length of the dest buffer.
 * @return true if the input was gotten successfully.
 */
int getTextInput(char *msg, char *dest, size_t bufflen);

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
