#ifndef SNAKE_H
#define SNAKE_H

#define SEGMENT_LIFE 100

/* the time the loop sleeps between the iterations */
static int SleepingTime = 100000;

/* the types a segment can have, and the character the type is rendered as */
enum SegmentType {HEAD = 'O', BODY = 'x', WALL_HORIZONTAL = '-',
	WALL_VERTICAL = '|', AIR = ' ', FOOD = '#'};

/* the directions the snake can move */
enum Directions {LEFT, RIGHT, UP, DOWN};

/**
 * A position with column/row coordinates.
 */
typedef struct Position {
	/* the coordinates */
	int column;
	int row;
} Position;


/**
 * A segment is placed on the playing board.
 */
typedef struct Segment {
	/* the position of the segment */
	Position p;

	/* the type of the segment (ie corner, body, wall) */
	enum SegmentType type;

	/* when 0, the segment will be altered to AIR */
	int lifeTicks;

	/* if true, the player will lose if hitting this segment */
	bool blocking;
} Segment;

/**
 * The playing board keeps track of the snake, the segments and the food 
 * (score). It also has width and height.
 */
typedef struct Board {
	/* the size of the board */
	int width;
	int height;

	/* the snake's body */
	Segment **segments;
	
	/* the snake's head */
	Segment head;

	/* the direction the snake is moving */
	enum Directions direction;

	/* the time each segment lives, in micro seconds */
	long segmentLivingTime;

	int food;
} Board;

/**
 * Initialize the board.
 * @param width The width of the playing board (in columns).
 * @param height The height of the playing board (in rows).
 * @return A Board * to the initialized board.
 */
Board *initGame(int width, int height);

/**
 * Perform inialization for ncurses.
 */
void initNCurses();

/**
 * Draw the snake at its current position.
 * @param b The board the snake is on.
 */
void drawSnake(Board *b);

/**
 * Checks if the position is occupied on the board. 
 * If the segment is non-air, it is.
 * @param p The position to check.
 * @param b The board to check on.
 * @return true if the position is occupied.
 */
bool positionIsOccupied(Position p, Board *b);

/**
 * Draw a border.
 * @param character The character to draw the border with.
 */
void drawBorder(char character);

/**
 * Generate food.
 * amount segments of type FOOD is placed on a random non-occupied position on the 
 * board.
 * @param b The board to place food on
 * @param amount The number of pieces to generate
 */
void generateFood(Board *b, int amount);

/**
 * Quit ncurses, print a message with the score and return 0.
 * @param b The board used for the game.
 */
void lose(Board *b);

/**
 * Checks if the head is out of bounds, ie hits the border.
 * @param b The board to check on.
 * @return True if the head hits the border, else false.
 */
bool headIsOutOfBoard(Board *b);

/**
 * Move the snake head in its direction.
 * @param b The board to move the snake on.
 */
void moveSnakeHead(Board *b);

/**
 * Update the moving direction, according to the result of getch().
 * @param b The board to update on.
 */
void updateMovingDirection(Board *b);

/**
 * Place a body segment at the head's position.
 * @param b The current playing board.
 */
void createBodySegmentAtHeadPosition(Board *b);

/**
 * The main game loop.
 * @param b The board of the current game.
 */
int gameLoop(Board *b);

#endif
