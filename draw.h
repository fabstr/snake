#include <curses.h>
#include "board.h"

#ifndef DRAW_H
#define DRAW_H

/**
 * Draw a border between x1, y, x2 and y2.
 */
void drawBorder(int x1, int y1, int x2, int y2);

void drawStats(Board *b);
void drawTextWindowInMiddle(char **text, int nrows);
void drawHighscore(Board *b);
void drawHelp();
int draw(Board *b, State *GameState);

#endif
