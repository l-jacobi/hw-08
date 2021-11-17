#include <iostream>
#include <fstream>
#include "huffman.hh"
#include "bitio.hh"
#include <cassert>

//takes a filename as a command-line argument, reads and compresses file into an output file of same name with .comp suffix
int main(int argc, char* argv[]){

	if(argc < 2){
		std::cerr << "Error: Not enough files entered" << std::endl;
	}
	
	for(int i = 1; i < argc; ++i){
		std::string input_file = argv[i];
		std::ifstream fis(input_file);
		if(!fis){ assert(false); }

		std::string output_file = input_file + ".comp";
		std::ofstream fos(output_file.c_str());
		if(!fos){ assert(false); }

		Huffman huff;	//creates huffman object for encoding
		BitOutput bito(fos);	//creates BitOutput object with ostream that writes to files

		int symbol = 0;	//symbol being encoded
		Huffman::bits_t bits;

		while(symbol != Huffman::HEOF){
			symbol = fis.get();	//get next symbol to be encoded

			if(fis.eof()){
				symbol = Huffman::HEOF;
			}
			for(bool bit : huff.encode(symbol)){
				bito.output_bit(bit);
			}
		}

		fos.close();
		fis.close();
	}
	return 0;
}