#include "segment.h"

void drawSegment(struct Segment *seg)
{
	/* give the character the correct color */
	int charToWrite = seg->drawingCharacter | seg->color_pair;
	
	/* draw the segment at its position */
	mvaddch(seg->p.row, seg->p.column, charToWrite);

}

/*void drawSegmentFromVoid(void *seg)*/
/*{*/
	/*drawSegment((struct Segment*) seg);*/
/*}*/
