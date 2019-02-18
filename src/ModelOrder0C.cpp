#include <list>
#include <tuple>
#include <iostream> // TODO: remove after testing
#include "ModelOrder0C.h"

ModelOrder0C::ModelOrder0C() {
	max_context_length = 4;
}

void ModelOrder0C::IncrementSymbolCount(Node *father, const unsigned char symbol) {
	if (father->children[symbol]) {
		//std::cout << "PLUS COUNT \n";
		father->children[symbol]->count++;
	} else {
		// Could not find a child node for the symbol. Create it
		Node *new_node = (Node*) malloc(sizeof(Node));
		new_node->symbol = symbol;
		new_node->count = 1;
		new_node->father = father;

		for(int i = 0; i < 256; i++){
			new_node->children[i] = NULL;
		}

		// Link it to the father node
		father->children[symbol] = new_node;
	}
}

std::tuple<unsigned int, unsigned int, unsigned int> ModelOrder0C::GetInterval(Node *father, unsigned char symbol) {
	unsigned int total = 1, low = 0, high = 1;

	// Cumulatively sum the node's count for each one that has been initialized...
	for(int i = 0; i < 256; i++){
		if (father->children[i]) {
			// ...and save interval boundries when the node's symbol is the one thas was read
			if(i == symbol){
				low = total;
				high = low + father->children[i]->count;
			}
			total += father->children[i]->count;
		}
	}

	return std::make_tuple(low, high, total);
}

void ModelOrder0C::UpdateTree(Node *root, unsigned char symbol, std::list<unsigned char> last_seen){
	unsigned short int max_depth = std::min<unsigned short int>(
			max_context_length,
			last_seen.size()
			);
	
		for (int context_size = 0; context_size <= max_depth; ++context_size) {
			Node *current_node = root;
			// Advance iterator to last_seen[context_size]
			std::list<unsigned char>::iterator last_seen_it = last_seen.begin();
			std::advance(last_seen_it, context_size);

			// Traverse down
			for(; last_seen_it != last_seen.end(); ++last_seen_it) {
				current_node = current_node->children[*last_seen_it];
			}	
			
			IncrementSymbolCount(current_node, symbol);
		}
}

unsigned long ModelOrder0C::Encode() {
	// Initialize PPM tree
	Node *tree = (Node*) malloc(sizeof(Node));
	for(int i = 0; i < 256; i++){
		tree->children[i] = NULL;
	}

	unsigned char buf;
	unsigned long symbol_count = 0;
	std::list<unsigned char> last_seen;
	while (mSource->read((char*)&buf, sizeof(char))) {
		unsigned short int max_depth = std::min<unsigned short int>(
			max_context_length,
			last_seen.size()
			);
		std::tuple<unsigned int, unsigned int, unsigned int> interval;
		bool encoded = false;

		// Start at the root and traverse down, up to the max context length (limited by last seen)
		for (int context_size = 0; context_size <= max_depth; ++context_size) {
			Node *current_node = tree;
			// Advance iterator to last_seen[context_size]
			std::list<unsigned char>::iterator last_seen_it = last_seen.begin();
			std::advance(last_seen_it, context_size);

			// Traverse down
			for(; last_seen_it != last_seen.end(); ++last_seen_it) {
				current_node = current_node->children[*last_seen_it];
			}

			if(!encoded){
				interval = GetInterval(current_node, buf);

				// Encode the symbol if it has been found in the context (low is not 0)
				if(std::get<0>(interval)){
					encoded = true;
					mAC.Encode(std::get<0>(interval), std::get<1>(interval), std::get<2>(interval));
					symbol_count++;
				}
				// Encode the escape if the symbol was not found but the context is filled (total is not 1)
				else if(std::get<2>(interval) > 1) {
					mAC.Encode(std::get<0>(interval), std::get<1>(interval), std::get<2>(interval));
					symbol_count++;
				}
			}

			IncrementSymbolCount(current_node, buf);
		}

		// If the symbol hasn't been encoded, a fixed prediction is made
		if(!encoded){
			mAC.Encode(buf, buf+1, 256);
			symbol_count++;
		}

		// Add symbol to last seen queue, remove furthest away symbol if queue is too big
		last_seen.push_back(buf);
		if (last_seen.size() > max_context_length) {
			last_seen.pop_front();
		}
	}

	return symbol_count;
}

void ModelOrder0C::Decode(unsigned long symbol_count) {
	// Initialize PPM tree
	Node *tree = (Node*) malloc(sizeof(Node));
	for(int i = 0; i < 256; i++){
		tree->children[i] = NULL;
	}

	// Pointer for tree navigation
	Node *ptr = tree;

	// Queue of K last seen symbols
	std::list<unsigned char> last_seen;

	// Start at context -1, where total is 256
	unsigned int mTotal = 256;
	short int cur_context = -1;

	unsigned int value, symbol, low, high;

	do {
		value = mAC.DecodeTarget(mTotal);

		// Translate range to symbol if not in context -1 or value was not escape
		if (value == 0) {
			symbol = value;
			low = 0;
			high = 1;
		} else if (cur_context == -1){
			symbol = value;
			low = value;
			high = value + 1;
		} else {
			high = 1;
			for (unsigned int i = 0; i < 256; ++i) {
				if (ptr->children[i]) {
					low = high;
					high += ptr->children[i]->count;

					if (value >= low && value < high) {
						symbol = i;
						break;
					}
				}
			}
		}

		mAC.Decode(low, high);

		if (symbol == 0) {
			// Escape. Go up a level in the tree 
			cur_context--;
			
			// Make sure that ptr doesn't go higher than the root
			if(cur_context != -1){
				ptr = ptr->father;
			}

		} else {
			// Received a valid symbol
			mTarget->write(reinterpret_cast<char*>(&symbol), sizeof(char));

			// If you were already in the root, don't go down!
			if(cur_context != -1){
				ptr = ptr->children[symbol];
			}

			cur_context++;
			UpdateTree(tree, symbol, last_seen);			
		}

		// Update mTotal to reflect the sum of counts in this context
		if (cur_context == -1) {
			mTotal = 256;
		} else {
			mTotal = 1;
			for (int i = 0; i < 256; ++i) {
				if (ptr->children[i]) {
					mTotal += ptr->children[i]->count;
				}
			}
		}

		if (symbol != 0) {
			// Add symbol to last seen queue, remove furthest away symbol if queue is too big
			last_seen.push_back(symbol);
			if (last_seen.size() > max_context_length) {
				last_seen.pop_front();
			}
		}
	} while (symbol_count--);
}
