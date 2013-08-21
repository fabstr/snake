#ifndef ASTAR_H
#define ASTAR_H

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "bheap.h"


typedef struct Node Node;
struct Node {
	int x;
	/**< @brief The x coordinate */
	
	int y;
	/**< @brief The y coordinate */

	bool penetrable;
	/**< @brief If true the x/y position can be passed */

	Node *parent;
	/**< @brief The parent node, or NULL if there is no parent node */

	int gScore;
	/**< @brief The best A* g score */

	int hScore;
	/**< @brief The A* h score */
};

typedef struct Neighbours Neighbours;
struct Neighbours {
	size_t count;
	Node **nodes;
};

/**
 * Construct a path from goal to start on for the array.
 * The path will be marked with the parent field of the Node structure. To find
 * the path, start at goalX/Y and follow *parent until it's NULL.
 * @param startX The x start coordinate
 * @param startY The y start coordinate
 * @param goalX The x goal coordinate
 * @param goalY The y goal coordinate
 * @param nodes A two dimensional array (though stored in one dimension) with 
 *              the properties nrows, ncols and rowWidth.
 * @param nrows The number of rows in the array
 * @param ncols The number of columns in the array
 * @param rowWidth The width of a row, in columns
 * @return The goal node, or NULL if there is no path.
 */
Node *getPath(int startX, int startY, int goalX, int goalY, Node nodes[], 
		int ncols, int nrows);

/**
 * Return true if the node is in the closedset.
 * It is if the node at node's position in closedset is not null.
 * @param closedset The closed set.
 * @param node The node to check for.
 * @param nrows The number of rows in the closedset array.
 * @return True if the node is in the closed set.
 */
bool inClosedSet(Node closedset[], Node *node, int nrows);

/**
 * Calculate the h score for the node.
 * @param node The current node (the one to calculate h score for)
 * @param goal The goal node
 * @return The h score
 */
int hScore(Node *node, Node *goal);

/**
 * Get the node at the given position.
 * @param x The x coordinate
 * @param y The y coordinate
 * @param nodes A two dimensional array (though stored in one dimension) with 
 *              the properties nrows, ncols and rowWidth.
 * @param nrows The number of rows in the array
 * @return The node, or NULL if it does not exist.
 */
Node *getNodeAt(int x, int y, Node nodes[], int nrows);

/** 
 * Calculate the h score for every node in the array and set the x/y variables.
 * The distance is the Manhattan distance and the h score is placed in the 
 * hScore field in the Nodes.
 * @param nodes A two dimensional array (though stored in one dimension) with 
 *              the properties nrows, ncols and rowWidth.
 * @param nrows The number of rows in the array
 * @param ncols The number of columns in the array
 * @param rowWidth The width of a row, in columns
 */
void calculateHScoresAndSetXY(int goalX, int goalY, Node nodes[], int nrows,
		int ncols);

/**
 * Compare the nodes e1 and e2 (which should be Node *) by the f score.
 * @param e1 The first node to compare
 * @param e2 The second node to compare
 * @return -1 if e1's f score < e2's f score, 0 if the f scores are equal, else
 *         1.
 */
int compareNodes(void *e1, void *e2);

/**
 * Calculate the f score of the node.
 * The g score should be up to date.
 * @param node The node
 * @return The f score
 */
int fScore(Node *node);

/**
 * Return the neighbours of node.
 * A neighbour is adjucent (top/left/right/down) to node, is penetrable and its
 * x/y position fits in nrows/ncols.
 * @param node The node to find neighbours of
 * @param nodes The nodes array
 * @param nrows The number of rows in the array
 */
Neighbours *getNeighbours(Node *node, Node nodes[], int nrows, int ncols);

/**
 * Free the Neighbours.
 * @param neighbours The Neighbours to free
 */
void freeNeighbours(Neighbours *neighbours);
#endif 
