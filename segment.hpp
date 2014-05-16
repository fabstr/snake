#ifndef SEGMENT_H
#define SEGMENT_H

#include <curses.h>
#include "position.h"
#include "stdlib.h"
#include "mlog.h"

/* the types a segment can have, and the character the type is rendered as */
enum SegmentType {HEAD, BODY, WALL, AIR, FOOD};

/**
 * A segment is placed on the playing board.
 */
class Segment {
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

public:
	/**
	 * Draw a segment on the screen.
	 * @param seg THe segment to draw.
	 */
	void draw();

	/**
	 * Constructor.
	 * @param y
	 * @param x
	 * @param type
	 * @param lifeTicks
	 * @param blocking
	 * @param drawingCharacter
	 * @param color_pair
	 */
	Segment(int y, int x, SegmentType type, int lifeTicks, bool blocking, 
			int drawingCharacter, int color_pair);

	Segment(SegmentType type, int drawingCharacter, int color_pair);

	/**
	 * Age the segment by amount.
	 * @param amount
	 */
	void decreaseLifeTicks(int amount);

	/**
	 * Decrease the age of the segment.
	 * @param amount
	 */
	void increaseLifeTicks(int amount);

	/**
	 * Get the life ticks left.
	 * @return The life ticks.
	 */
	int getLifeTicks();

	/**
	 * Get the segments position.
	 * @return The position.
	 */
	Position getPosition();
	
	/**
	 * Get the color pair.
	 * @return The color pair.
	 */
	int getColorPair();

	/**
	 * Set the position.
	 * @param p The position
	 */
	void setPosition(Position p);

	/**
	 * Move the segment left.
	 */
	void moveLeft();

	/**
	 * Move the segment right.
	 */
	void moveRight ();

	/**
	 * Move the segment up.
	 */
	void moveUp();

	/**
	 * Move the segment down.
	 */
	void moveDown();
		
};

#endif
