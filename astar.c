#include "astar.h"


Node *getPath(int startX, int startY, int goalX, int goalY, Node nodes[], 
		int ncols, int nrows)
{
	Node *start = getNodeAt(startX, startY, nodes, nrows);
	start->x = startX;
	start->y = startY;
	start->gScore = 0;
	start->hScore = hScore(start, goal);
	start->parent = NULL;

	Node *goal = getNodeAt(goalX, goalY, nodes, nrows);
	goal->x = goalX;
	goal->y = goalY;

	calculateHScoresAndSetXY(goalX, goalY, nodes, nrows, ncols);

	// closedset := the empty set
	Node closedset[nrows*ncols];
	memset(closedset, '\0', nrows*ncols*sizeof(Node));

	// openset := {start}
	BHeap *openset = newBHeap(ncols*nrows, compareNodes, MIN);
	addElement(openset, start);
	
	Node *current = start;
	do {
		// if current = goal
		if (current == goal) {
			// return reconstruct_path(came_from, goal)
			freeBHeap(openset);
			return current;
		}

		// add current to closedset
		closedset[current->x*nrows + current->x] = *current;

		mlog("picking current x=%d y=%d, g=%d h=%d", current->x, current->y, current->gScore, current->hScore);

		// for each neighbour in neighbor_nodes(current)
		Neighbours *neighbours = getNeighbours(current, nodes, nrows,
				ncols);
		for (int i=0; i<neighbours->count; i++) {
			Node *n = neighbours->nodes[i];
			
			// tentative_g_score := gscore(current) +
			//         dist_between(current, neighbour)
			int tentativeGScore = current->gScore + 1;

			// if neighbour in closedset and tentative_g_score >=
			//         gscore(neighbour)
			if (inClosedSet(closedset, n, nrows) == true &&
					tentativeGScore >= n->gScore) {
				// continue
				continue;
			} 
			
			// if neighbour not in openset or tentative_g_score <
			//         gscore(neighbour)
			if (tentativeGScore < n->gScore ||
					exists(openset, n) == false) {
				// came_from(neighbour) := current
				n->parent = current;

				// gscore(neighbour) := tentative_g_score
				n->gScore = tentativeGScore;

				// if neighbour not in openset
				if (exists(openset, n) == false) {
					// addd neighbour to openset
					addElement(openset, n);
				}
			}
		}
		freeNeighbours(neighbours);
		
		// current := the node in openset having the lowerst fscore
		// remove current from openset
	} while ((current = removeTop(openset)) != NULL);
	
	freeBHeap(openset);

	// return failure
	return NULL;
}

bool inClosedSet(Node closedset[], Node *node, int nrows)
{
	Node *nodeAt = getNodeAt(node->x, node->y, closedset, nrows);
	return (nodeAt == NULL) ? true : false;
}

int hScore(Node *node, Node *goal)
{
	int xDist = goal->x - node->x;
	int yDist = goal->y - node->y;

	return abs(xDist) + abs(yDist);
}

Node *getNodeAt(int x, int y, Node nodes[], int nrows)
{
	return &nodes[y*nrows + x];
}

void calculateHScoresAndSetXY(int goalX, int goalY, Node nodes[], int nrows,
		int ncols)
{
	Node *goal = getNodeAt(goalX, goalY, nodes, nrows);
	goal->x = goalX;
	goal->y = goalY;

	if (goal == NULL) {
		return;
	}

	for (int y=0; y<nrows; y++) {
		for (int x=0; x<ncols; x++) {
			Node *currentNode = getNodeAt(x, y, nodes, nrows);
			if (currentNode == NULL) {
				continue;
			}

			currentNode->x = x;
			currentNode->y = y;
			currentNode->gScore = INT_MAX;
			currentNode->hScore = hScore(currentNode, goal);
		}
	}
}

int compareNodes(void *e1, void *e2)
{
	Node *n1 = (Node *) e1;
	Node *n2 = (Node *) e2;
	if (fScore(n1) < fScore(n2)) {
		return -1;
	} else if (fScore(n1) == fScore(n2)) {
		return 0;
	} else {
		return 1;
	}
}

int fScore(Node *node)
{
	return node->gScore + node->hScore;
}

Neighbours *getNeighbours(Node *node, Node nodes[], int nrows, int ncols)
{
	Neighbours *neighbours = (Neighbours *) malloc(sizeof(Neighbours));

	/* at most there are 4 neighbours */
	neighbours->nodes = (Node **) malloc(4*sizeof(Node *));

	neighbours->count = 0;

	int x, y;
	size_t count = 0;
	Node *currentNeighbour;

	/* above */
	x = node->x;
	y = node->y-1;
	if (x < ncols && y < nrows) {
		currentNeighbour = getNodeAt(x, y, nodes, nrows);
		if (currentNeighbour->penetrable == true) {
			neighbours->nodes[count++] = currentNeighbour;
		}
	}

	/* below */
	x = node->x;
	y = node->y+1;
	if (x < ncols && y < nrows) {
		currentNeighbour = getNodeAt(x, y, nodes, nrows);
		if (currentNeighbour->penetrable == true) {
			neighbours->nodes[count++] = currentNeighbour;
		}
	}

	/* left */
	x = node->x-1;
	y = node->y;
	if (x < ncols && y < nrows) {
		currentNeighbour = getNodeAt(x, y, nodes, nrows);
		if (currentNeighbour->penetrable == true) {
			neighbours->nodes[count++] = currentNeighbour;
		}
	}

	/* right */
	x = node->x+1;
	y = node->y;
	if (x < ncols && y < nrows) {
		currentNeighbour = getNodeAt(x, y, nodes, nrows);
		if (currentNeighbour->penetrable == true) {
			neighbours->nodes[count++] = currentNeighbour;
		}
	}

	neighbours->count = count;
	return neighbours;
}

void freeNeighbours(Neighbours *neighbours)
{
	free(neighbours->nodes);
	free(neighbours);
}
