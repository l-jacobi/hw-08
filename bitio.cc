#include "bitio.hh"
#include <cassert>
#include <climits>

BitInput::BitInput(std::istream& is){
	is_ = &is;			//input stream in use
	symbol_ = is_->get();		//the symbol which bits are being read from
	bit_offset_ = 1;	//determines which binary place bits are read from
}

//bits are read per character left to right, most to least significant
bool BitInput::input_bit(){
	assert(is_ && !is_->eof());	//ensures is_ != nullptr, no more inputs
	//if previous character is finished being read, go to next char
	if(bit_offset_ > CHAR_BIT){
		symbol_ = is_->get();	//read next character to convert to bits from istream
		bit_offset_ = 1;		//reset bit offset
	}
	char_int_t mask = 1 << (CHAR_BIT - bit_offset_); //mask is an int with a 1 in its (CHAR_BIT - place_) place when in binary form
	bool bit = symbol_ & mask;	//true if symbol has 1 in ^ place in binary form, false if 0
	++bit_offset_;
	return bit;
}

BitOutput::BitOutput(std::ostream& os){
	os_ = &os;			//output stream in use
	symbol_ = 0;		//current symbol starts with all zeros
	bits_written_ = 0;	//how many bits have been written into current character
}


//ensures final symbol is finished, and if it's not... finish it anyways
BitOutput::~BitOutput(){
	if(symbol_ != 0){	//if current symbol is unfinished
		symbol_ = symbol_ << (CHAR_BIT - bits_written_); //adds trailing zeros? not sure abt that instruction, shouldve asked for clearer instructions but i accidentally slept through the second lab lol
		os_->put(symbol_);	//flushes current bits to ostream? not sure what "flush" instruction meant, see comment ^
	}
}

//bits are written per character left to right, most to least significant
void BitOutput::output_bit(bool bit){
	assert(os_);	//os != nullptr
	symbol_ = symbol_ << 1;		//make space for new bit
	symbol_ = symbol_ | bit;	//add new bit to symbol_ in the 1s place
	++bits_written_;

	if(bits_written_ == CHAR_BIT){	//if entire character is written
		os_->put(symbol_);	//write symbol to ostream
		bits_written_ = 0;	//reset bit offset
		symbol_ = 0;		//reset symbol
	}
}