#include "segment.h"

void drawSegment(Segment *seg)
{
	/* give the character the correct color */
	int charToWrite = seg->drawingCharacter | seg->color_pair;
	
	/* draw the segment at its position */
	mvaddch(seg->p.row, seg->p.column, charToWrite);

}

void drawSegmentFromVoid(void *seg)
{
	drawSegment((Segment*) seg);

	Segment *s = (Segment *) seg;
	mlog("drawing segment at y=%d x=%d", s->p.row, s->p.column);
}
