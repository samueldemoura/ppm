#ifndef __NODE_H__
#define __NODE_H__

/*
 * Basic node struct used during encoding
 */
typedef struct Node {
	char symbol;
	unsigned int count;

	Node *children[256];
} Node;

/*
 * Slightly less RAM-friendly node struct used during decoding
 */
typedef struct DecNode {
	char symbol;
	unsigned int count;

	DecNode *children[256];
	DecNode *father;
} DecNode;

#endif
