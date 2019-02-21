#ifndef __MODELI_H__
#define __MODELI_H__

#include "ArithmeticCoderC.h"

enum ModeE
{
	MODE_ENCODE = 0,
	MODE_DECODE
};

class ModelI
{
public:
	void Process( fstream *source, fstream *target, ModeE mode );
	unsigned short int max_context_length;

protected:
	virtual unsigned long long Encode() = 0;
	virtual void Decode(unsigned long long symbol_count) = 0;

	ArithmeticCoderC mAC;
	fstream *mSource;
	fstream *mTarget;
};

#endif
