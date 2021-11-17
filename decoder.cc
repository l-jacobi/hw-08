#include <iostream>
#include <fstream>
#include "huffman.hh"
#include "bitio.hh"
#include <cassert>

//takes a filename as a command-line argument, reads and compresses file into an output file of same name with .comp suffix
int main(int argc, char* argv[]){

	if(argc < 2){ std::cerr << "Error: Not enough files entered" << std::endl; }
	
	for(int i = 1; i < argc; ++i){

		std::string input_file = argv[i];
		std::ifstream fis(input_file);
		if(!fis){ assert(false); }

		std::string output_file = std::string(argv[i]) + ".plaintext";
		std::ofstream fos(output_file.c_str());
		if(!fos){ assert(false); }

		Huffman huff;	//creates huffman object for decoding
		BitInput biti(fis);	//creates BitInput object with istream type that inputs from files

		int symbol = 0;	//symbol being decoded

		while(symbol != Huffman::HEOF){
			while(!fis.eof() && ((symbol = huff.decode(biti.input_bit())) < 0));	//code for evil and insane people
			if(fis.eof()){
				symbol = Huffman::HEOF;
			} else{
			//if(symbol != Huffman::HEOF){
				fos.put(symbol);
			}
		}

		fos.close();
		fis.close();
	}
	return 0;
}
