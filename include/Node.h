#ifndef __NODE_H__
#define __NODE_H__

/*
 * Basic node struct used during encoding and decoding
 */
typedef struct Node {
	char symbol;
	unsigned int count;

	Node *children[256];
} Node;

#endif
