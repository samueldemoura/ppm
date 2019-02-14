#include <list>
#include "ModelOrder0C.h"

ModelOrder0C::ModelOrder0C() {
	max_context_length = 4;
}

void ModelOrder0C::IncrementSymbolCount(Node *father, const char symbol, bool found, int found_index) {
	/*if (found) {
		father->children[found_index]->count++;
	} else {
		// Could not find a child node for the symbol. Create it:
		Node *new_node = (Node*) malloc(sizeof(Node));
		new_node->symbol = symbol;
		new_node->count = 1;

		// Link it to the father node
		father->children[reinterpret_cast<unsigned short int>(&symbol)] = new_node;
	}*/
}

void ModelOrder0C::FindContext(Node *father, char symbol, bool &encoded) {
	// TODO: fix this. completely broken atm
	bool found = false;
	unsigned int total = 1, low = 0, high = 1;

	if (father_node->children[(unsigned int)symbol]) {
		// TODO: does this "non-null" check-if actually work? Behaviour I want is: only go into
		// this block if child[symbol] is an already instantiated, non-null Node struct
		found = true;
		low = total;
		high = low + father_node->children[(unsigned int)symbol]->count;
	}
}

void ModelOrder0C::Encode() {
	// Initialize PPM tree
	Node *tree = (Node*) malloc(sizeof(Node));

	char buf;
	std::list<char> last_seen;
	while (mSource->read(&buf, sizeof(char))) {
		bool encoded = false;
		unsigned short int max_depth = std::min<unsigned short int>(
			max_context_length,
			last_seen.size()
			);

		// Start at the root and traverse down, up to the max context length (limited by last seen)
		for (int context_size = 0; context_size <= max_depth; ++context_size) {
			Node *current_node = tree;

			// Advance iterator to last_seen[context_size]
			std::list<char>::iterator last_seen_it = last_seen.begin();
			std::advance(last_seen_it, context_size);

			// Traverse down
			for(; last_seen_it != last_seen.end(); ++last_seen_it) {
				current_node = current_node->children[(unsigned int)*last_seen_it];
			}

			FindContext(current_node, buf, encoded);
		}

		// If the context hasn't been found through all tries, then a fixed prediction is made
		if (!encoded) {
			mAC.Encode(buf, buf+1, 255); // TODO: figure out if this should be 255 or 256
			//encode_symbol(buf, buf+1, MAX_BYTE_VALUE, &outfile);
		}

		// Add symbol to last seen queue, remove furthest away symbol if queue is too big
		last_seen.push_back(buf);
		if (last_seen.size() > max_context_length) {
			last_seen.pop_front();
		}
	}
}

void ModelOrder0C::Decode()
{
	// TODO
	/*unsigned int symbol;

	do
	{
		unsigned int value;

		// Wert lesen
		value = mAC.DecodeTarget( mTotal );

		unsigned int low_count = 0;

		// Symbol bestimmen
		for( symbol=0; low_count + mCumCount[symbol] <= value; symbol++ )
			low_count += mCumCount[symbol];

		// Symbol schreiben
		if( symbol < 256 )
			mTarget->write( reinterpret_cast<char*>(&symbol), sizeof( char ) );

		// Dekoder anpassen
		mAC.Decode( low_count, low_count + mCumCount[ symbol ] );

		// update model
		mCumCount[ symbol ]++;
		mTotal++;
	}
	while( symbol != 256 );*/
}
