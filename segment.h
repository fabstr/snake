#include <curses.h>

#include "position.h"
#include "mlog.h"
#include "stdlib.h"

#ifndef SEGMENT_H
#define SEGMENT_H

/* the types a segment can have, and the character the type is rendered as */
enum SegmentType {HEAD, BODY, WALL, AIR, FOOD};

/**
 * A segment is placed on the playing board.
 */
typedef struct Segment {
	/* the position of the segment */
	Position p;

	/* the type of the segment (ie corner, body, wall) */
	enum SegmentType type;

	/* when 0, the segment will be removed */
	int lifeTicks;

	/* if true, the player will lose if hitting this segment */
	bool blocking;

	/* the ncurses drawing character of the segment */
	int drawingCharacter;

	/* the ncurses color of the segment */
	int color_pair;
} Segment;

/**
 * Draw a segment on the screen.
 * @param seg THe segment to draw.
 */
void drawSegment(Segment *seg);

/**
 * Like drawSegment but seg comes from a void*.
 * @param seg The segment to draw.
 */
void drawSegmentFromVoid(void *seg);

/**
 * Free the segment.
 * @param seg The segment to free.
 */
void freeSegment(Segment *seg);
#endif
