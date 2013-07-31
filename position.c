/**
 * Filename: Position.c
 *
 * Description: 
 *
 * Version: 1.0
 * Created: 30.07.2013 23:21:09
 *
 * Author: Fabian Ström (fs), fs@fabianstrom.se
 */

#include "position.h"

bool positionEqual(Position p1, Position p2)
{
	return p1.row == p2.row && p1.column == p2.column;
}
