#ifndef DRAW_H
#define DRAW_H

#include <curses.h>
#include "board.hpp"
#include "state.h"
#include "board.hpp"

/**
 * Draw a border between x1, y, x2 and y2.
 */
void drawBorder(int x1, int y1, int x2, int y2);

void drawStats(int score);
void drawTextWindowInMiddle(char **text, int nrows);
void drawHelp();
void clearScreen();
void refreshScreen();

#endif
