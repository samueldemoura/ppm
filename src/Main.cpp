#include <iostream>
#include <fstream>
#include "ModelOrder0C.h"

// signature: "ACMC" (0x434D4341, intel byte order)
const int g_Signature = 0x434D4341;

int main(int argc, char *argv[]) {
	// Make sure we received the necessary args
	if (argc < 3 || argc > 4) {
		std::cout << "Usage: ./ppmc <input file> <output file> <k (if encoding)>\n";
		return 1;
	}

	// Open input and output files
	std::fstream infile(argv[1], std::ios::in | std::ios::binary);
	std::fstream outfile(argv[2], std::ios::out | std::ios::binary);

	if (!infile.is_open() || !outfile.is_open()) {
		std::cout << "Error opening file.\n";
		return 1;
	}

	// Check if input file begins with our signature
	unsigned int signature;
	infile.read(reinterpret_cast<char*>(&signature),sizeof(signature));

	ModelOrder0C *model = new ModelOrder0C;

	if (signature == g_Signature) {
		// Read context size from file
		infile.read(
			reinterpret_cast<char*>(&model->max_context_length),
			sizeof(unsigned short int)
			);

		// Decode
		model->Process(&infile, &outfile, MODE_DECODE);
	} else {
		// Rewind to beginning of input file, write signature to output
		infile.seekg(0, std::ios::beg);
		outfile.write(reinterpret_cast<const char*>(&g_Signature), sizeof(g_Signature));

		// Write context size to file (use default k=4 if not specified by user)
		if (argc == 4) {
			model->max_context_length = std::stoi(argv[3]);
		} else {
			model->max_context_length = 4;
		}

		outfile.write(
			reinterpret_cast<const char*>(&model->max_context_length),
			sizeof(unsigned short int)
			);

		// Encode
		model->Process(&infile, &outfile, MODE_ENCODE);
	}

	outfile.close();
	infile.close();
	return 0;
}
