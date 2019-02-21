#include "ModelI.h"
#include <iostream>

void ModelI::Process( fstream *source, fstream *target, ModeE mode)
{
	mSource = source;
	mTarget = target;
	unsigned long long symbol_count = 0;

	if( mode == MODE_ENCODE )
	{
		mAC.SetFile( mTarget );
		
		// Save the space on file to write the total number of symbols ecoded later
		mTarget->write(reinterpret_cast<char*>(&symbol_count), sizeof(unsigned long long));

		// Encode
		symbol_count = Encode();

		mAC.EncodeFinish();

		// Go back to the position saved and write the symbol count
		mTarget->seekg(sizeof(int) + sizeof(unsigned short int), mTarget->beg);
		mTarget->write(reinterpret_cast<char*>(&symbol_count), sizeof(unsigned long long));
	}
	else // MODE_DECODE
	{
		
		mAC.SetFile( mSource );

		// Read the total number of symbols to decode
		mSource->read(reinterpret_cast<char*>(&symbol_count), sizeof(unsigned long long));

		mAC.DecodeStart();

		// Decode
		Decode(symbol_count);
	}
};