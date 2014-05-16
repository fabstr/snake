#include "snake.hpp"

void Snake::move(enum Directions d)
{
	/* update previousDirection if neccessary */
	if (direction != d) {
		previousDirection = direction;
		direction = d;
	}

	/* move the head */
	switch (d) {
	case LEFT:
		mlog("moving snake left");
		head->moveLeft();
		break;
	case UP:
		mlog("moving snake up");
		head->moveUp();
		break;
	case RIGHT:
		mlog("moving snake right");
		head->moveRight();
		break;
	case DOWN:
		mlog("moving snake down");
		head->moveDown();
		break;
	}
}

void Snake::age()
{
	for (list<Segment>::iterator seg = body.begin();
			seg != body.end();
			++seg) {
		seg->decreaseLifeTicks(growthSpeed);
	}

	destroyOldBodySegments();
}

void Snake::destroyOldBodySegments()
{
	for (list<Segment>::iterator seg = body.end();
			seg != body.begin();
			--seg) {
		if (seg->getLifeTicks() <= 0) {
			body.pop_back();
		}
	}
}

void Snake::draw()
{
	for (list<Segment>::iterator seg = body.begin();
			seg != body.end();
			++seg) {
		seg->draw();
	}

	drawHead();
}

void Snake::drawHead()
{
	/* draw the head */
	int headChar;
	switch (direction) {
	case LEFT:
		headChar = ACS_LARROW;
		break;
	case RIGHT:
		headChar = ACS_RARROW;
		break;
	case UP:
		headChar = ACS_UARROW;
		break;
	case DOWN:
		headChar = ACS_DARROW;
		break; }

	Position p = head->getPosition();
	mvaddch(p.row, p.column, headChar | head->getColorPair());
}

Snake::Snake(int y, int x)
{
	Snake(y, x, 0, 1);
}

Snake::Snake(int y, int x, int segmentLivingTime, int growthSpeed)
{
	this->segmentLivingTime = segmentLivingTime;
	this->growthSpeed = growthSpeed;

	mlog("snake constructor: y=%d x=%d", y, x);

	reset(y, x);

}

void Snake::reset(int y, int x)
{
	mlog("reset snake at y=%d x=%d", y, x);

	body.clear();
	direction = UP;
	previousDirection = UP;
	score = 0;
	createHead(y, x);
}

Snake::~Snake()
{
	delete head;
	body.clear();
}

bool Snake::isPositionOccupied(Position p)
{
	for (list<Segment>::iterator seg = body.begin();
			seg != body.end();
			++seg) {
		if (positionEqual(p, seg->getPosition())) {
			mlog("isPositionOccupied: row=%d col=%d", p.row, p.column);
			logBody();
			return true;
		}
	}

	return false;
}

void Snake::addSegmentAtHeadsPosition()
{

	if (segmentLivingTime <= 0) {
		// we don't want to add a segment
		return;
	}

	/* give the segment a correct drawing character (curses corners/lines) 
	 */
	int drawingCharacter = 0;
	if (direction != previousDirection) {
		/* the snake is changing direction */
		if (previousDirection == RIGHT && direction == DOWN) {
			drawingCharacter = ACS_URCORNER;
		} else if (previousDirection == RIGHT && direction
				== UP) {
			drawingCharacter = ACS_LRCORNER;
		} else if (previousDirection == DOWN && direction
				== RIGHT) {
			drawingCharacter = ACS_LLCORNER;
		} else if (previousDirection == DOWN && direction
				== LEFT) {
			drawingCharacter = ACS_LRCORNER;
		} else if (previousDirection == LEFT && direction
				== UP) {
			drawingCharacter = ACS_LLCORNER;
		} else if (previousDirection == LEFT && direction
				== DOWN) {
			drawingCharacter = ACS_ULCORNER;
		} else if (previousDirection == UP && direction
				== LEFT) {
			drawingCharacter = ACS_URCORNER;
		} else if (previousDirection == UP && direction
				== RIGHT) {
			drawingCharacter = ACS_ULCORNER;
		}
	} else if (direction == UP || direction == DOWN) {
		drawingCharacter = ACS_VLINE;
	} else if (direction == LEFT || direction == RIGHT) {
		drawingCharacter = ACS_HLINE;
	}

	/* construct the segment */
	Position p = head->getPosition();


	mlog("adding segment at row=%d column=%d seglivtime=%ld", p.row, p.column, segmentLivingTime);
	Segment seg = Segment(p.row, p.column, BODY,
			growthSpeed+segmentLivingTime, true, drawingCharacter,
			COLOR_PAIR(BODY_COLOR));

	/* add the segment */
	body.push_front(seg);
}

void Snake::createHead(int y, int x)
{
	head = new Segment(y, x, HEAD, 0, true, 'O', COLOR_PAIR(HEAD_COLOR));
}

void Snake::update()
{
	addSegmentAtHeadsPosition();
	age();

	// move must take place last or we will not move away from a newly added
	// segment
	move(direction);

	previousDirection = direction;
}

void Snake::setDirection(enum Directions d)
{
	/* save the old direction */
	previousDirection = direction;

	switch (d) {
	case LEFT:
		if (direction != RIGHT) {
			direction = LEFT;
		}
		break;
	case RIGHT:
		if (direction != LEFT) {
			direction = RIGHT;
		}
		break;
	case DOWN:
		if (direction != UP) {
			direction = DOWN;
		}
		break;

	case UP:
		if (direction != DOWN) {
			direction = UP;
		}
		break;
	}
}

void Snake::getLocalInput(State *GameState)
{
	/* get the new direction */
	int ch = getch();
	switch (ch) {
	case KEY_LEFT:
		setDirection(LEFT);
		break;
	case KEY_RIGHT:
		setDirection(RIGHT);
		break;
	case KEY_DOWN:
		setDirection(DOWN);
		break;
	case KEY_UP:
		setDirection(UP);
		break;
	case 'p':
		*GameState = (*GameState == PAUSED) ? PLAYING : PAUSED;
		break;
	case 'h':
		*GameState = (*GameState == HELP) ? PLAYING : HELP;
		break;
	case 'q':
		*GameState = QUIT;
		break;
	case 't':
		*GameState = (*GameState == HIGHSCORE) ?  PLAYING : HIGHSCORE;
		break;
	}
}

void Snake::getNetworkInput(Connection *c, State *GameState)
{
	mlog("getting network input");
	Message *m = listenForMessage(c);
	mlog("got '%s'", m->msg);
	char messageContent[m->length];
	sscanf(m->msg, "INPUT %s", messageContent);

	if (strcmp("up", messageContent) == 0) {
		setDirection(UP);
	} else if (strcmp("down", messageContent) == 0) {
		setDirection(DOWN);
	} else if (strcmp("left", messageContent) == 0) {
		setDirection(LEFT);
	} else if (strcmp("right", messageContent) == 0) {
		setDirection(RIGHT);
	} else if (strcmp("pause", messageContent) == 0) {
		*GameState = (*GameState == PAUSED) ? PLAYING : PAUSED;
	} else if (strcmp("highscore", messageContent) == 0) {
		*GameState = (*GameState == HIGHSCORE) ?  PLAYING : HIGHSCORE;
	} else if (strcmp("quit", messageContent) == 0) {
		*GameState = QUIT;
	} else if (strcmp("help", messageContent) == 0) {
		*GameState = (*GameState == HELP) ? PLAYING : HELP;
	}
}

void Snake::increaseLength()
{
	for (list<Segment>::iterator seg = body.begin();
			seg != body.end();
			++seg) {
		seg->increaseLifeTicks(growthSpeed);
	}
}

Segment *Snake::getHead()
{
	return head;
}

void Snake::increaseScore(int amount)
{
	score += amount;
}

void Snake::increaseSegmentLivingTime(int amount)
{
	mlog("increase segment living time %d", amount);
	segmentLivingTime += amount;
}

int Snake::getGrowthSpeed()
{
	return growthSpeed;
}

int Snake::getScore()
{
	return score;
}

void Snake::logBody()
{
	for (list<Segment>::iterator seg = body.begin();
			seg != body.end();
			++seg) {
		Position p = seg->getPosition();
		mlog("Segment at row=%3d col=%3d", p.row, p.column);
	}
}


void Snake::eatFood()
{
	increaseScore(1);
	increaseSegmentLivingTime(growthSpeed);
	increaseLength();
}
