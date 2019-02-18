#ifndef __MODELORDER0C_H__
#define __MODELORDER0C_H__

#include "ModelI.h"
#include "Node.h"
#include <list>

class ModelOrder0C : public ModelI
{
public:
	ModelOrder0C();

protected:
	unsigned long Encode();
	void Decode(unsigned long symbol_count);

	void IncrementSymbolCount(Node *father, unsigned char symbol);
	std::tuple<unsigned int, unsigned int, unsigned int> GetInterval(Node *father, unsigned char symbol);
	void UpdateTree(Node *root, unsigned char symbol, std::list<unsigned char> last_seen);
};

#endif
