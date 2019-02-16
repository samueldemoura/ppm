#ifndef __MODELORDER0C_H__
#define __MODELORDER0C_H__

#include "ModelI.h"
#include "Node.h"

class ModelOrder0C : public ModelI
{
public:
	ModelOrder0C();

protected:
	void Encode();
	void Decode();

	void IncrementSymbolCount(Node *father, unsigned char symbol);
	void IncrementSymbolCount(DecNode *father, unsigned char symbol);
	std::tuple<unsigned int, unsigned int, unsigned int> GetInterval(Node *father, unsigned char symbol);
};

#endif
