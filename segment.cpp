#include "segment.hpp"

Segment::Segment(int y, int x, SegmentType type, int lifeTicks, bool blocking, 
	int drawingCharacter, int color_pair)
{
	this->p.row = y;
	this->p.column = x;
	this->type = type;
	this->lifeTicks = lifeTicks;
	this->blocking = blocking;
	this->drawingCharacter = drawingCharacter;
	this->color_pair = color_pair;
	mlog("new segment at row=%d col=%d lt=%d", y, x, lifeTicks);
}

void Segment::draw()
{
	/* give the character the correct color */
	int charToWrite = drawingCharacter | color_pair;
	
	/* draw the segment at its position */
	mvaddch(p.row, p.column, charToWrite);

}

void Segment::decreaseLifeTicks(int amount)
{
	mlog("ticking life with %d, my id is %ld", amount, (long) this);
	lifeTicks -= amount;
}

void Segment::increaseLifeTicks(int amount)
{
	lifeTicks += amount;
}

int Segment::getLifeTicks()
{
	return lifeTicks;
}

Position Segment::getPosition()
{
	return p;
}

int Segment::getColorPair()
{
	return color_pair;
}

Segment::Segment(SegmentType type, int drawingCharacter, int color_pair)
{
	this->type = type;
	this->drawingCharacter = drawingCharacter;
	this->color_pair = color_pair;
}

void Segment::setPosition(Position p)
{
	this->p = p;
}

void Segment::moveLeft()
{
	p.column--;
}

void Segment::moveRight()
{
	p.column++;
}

void Segment::moveUp()
{
	p.row--;
}

void Segment::moveDown()
{
	p.row++;
}

