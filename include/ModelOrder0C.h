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

	void IncrementSymbolCount(Node *father, char symbol, bool found, int found_index);
	void FindContext(Node *father, char symbol, bool &encoded);
};

#endif
