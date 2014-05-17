#include "board.hpp"

void Board::generateFood()
{
	srandom(time(NULL));
	bool generating = true;
	while (generating) {
		/* get the limits for the border */
		int row = (int) random() % (this->height - 4);
		int col = (int) random() % (this->width - 2);
		row++; 
		col++;

		/* construct the current position */
		Position pos = {.row=row, .column=col};

		if (snake->isPositionOccupied(pos) == false) {
			foodSegment->setPosition(pos);
			break;
		}
	}
}

bool Board::headIsOutOfBoard()
{
	/* the head's position */
	Position hp = snake->getHead()->getPosition();

	if (hp.row < 1 || hp.row >= height-1 || hp.column < 1 ||
			hp.column >= width-1) {
		mlog("head is out of board, head is column=%d row=%d width=%d height=%d", 
				hp.column, hp.row, width, height);
		return true;
	}

	return false;
}

void Board::update()
{
	snake->update();
	checkForFood();
}

bool Board::hasPlayerLost()
{
	/* check if the snake hits itself */
	if (snake->isPositionOccupied(snake->getHead()->getPosition()) 
			== true) {
		/* there is something at the head */
		mlog("head is occupied");
		return true;
	} else if (headIsOutOfBoard() == true) {
		mlog("head is out of board");
		return true;
	} else {
		return false;
	}
}

void Board::resetGame()
{
	/* reset the snake */
	snake->reset(height/2, width/2);

	/* add some food */
	generateFood();
}

void Board::checkForFood()
{
	if (positionEqual(foodSegment->getPosition(), 
				snake->getHead()->getPosition())
			== true) {
		snake->eatFood();
		generateFood();
	}
}

void Board::addSnake(Snake *s)
{
	snake = s;
}

int Board::getWidth()
{
	return width;
}

int Board::getHeight()
{
	return height;
}

Board::Board(int w, int h, string *highscorePath, int growthSpeed, 
	int movementSpeed)
{
	// set width and height
	width = w;
	height = h;

	mlog("board: setting width=%d height=%d", width, height);

	// set the highscore table
	HighscoreTable *high = new HighscoreTable(highscorePath);
	if (high == NULL) {
		endwin();
		throw new FatalException("Highscore could not load.");
	} else {
		highscore = high;
	}

	// add the snake
	mlog("snake head at height/2=%d widht/2=%d", height/2, width/2);
	Snake *s = new Snake(height/2, width/2, 0, growthSpeed);
	addSnake(s);

	// set the food segment
	foodSegment = new Segment(FOOD, ACS_DIAMOND, COLOR_PAIR(FOOD_COLOR));

	// use the reset function to init the rest
	resetGame();
}

Board::~Board()
{
	delete highscore;
	delete foodSegment;
	delete snake;

	if (listenConnection != NULL) {
		free(listenConnection);
	}
}

void Board::setListenConnection(Connection *c)
{
	listenConnection = c;
}

void Board::setRemote(bool value)
{
	remote = value;
}

void Board::setAi(bool value)
{
	ai = value;
}

Snake *Board::getSnake()
{
	return snake;
}

void Board::writeHighscore(string *path)
{
	highscore->writeHighscoreTable(path);
}

bool Board::getRemote()
{
	return remote;
}

bool Board::getAi()
{
	return ai;
}

Connection *Board::getListenConnection()
{
	return listenConnection;
}

void Board::draw(State *GameState)
{	
	clearScreen();

	/* draw the score window */
	drawBorder(0, LINES-3, COLS-1, LINES-1);
	drawStats(getSnake()->getScore());

	/* draw the border around everything */
	drawBorder(0, 0, COLS-1, LINES-1);
	foodSegment->draw();
	snake->draw();

	string textarr[2];
	switch (*GameState) {
	case HELP:
		drawHelp();
		break;
	case HIGHSCORE:
		mlog("board: drawing highscore");
		getHighscore()->draw();
		mlog("board: done drawing highscore");
		break;
	case PAUSED:
		textarr[0] = "The game is paused.";
		textarr[1] = "Press p to continue.";
		drawTextWindowInMiddle(textarr, 2);
		break;
	default:
		break;
	}

	refreshScreen();
}

HighscoreTable *Board::getHighscore()
{
	return highscore;
}
