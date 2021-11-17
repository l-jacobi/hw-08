/*
 * Unit tests for class BitIO
 */

#include "bitio.hh"
#include <cassert>
#include <sstream>
#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////////
void test_1_bit(){
	std::stringstream bits;

	{
		BitOutput bito(bits);
		bito.output_bit(1);
	} //bito destructor called here
//	std::cout << "after destructor called here?\n"; //yes

	BitInput biti(bits);
	assert(biti.input_bit() == true);
	assert(biti.input_bit() == false);  // Should just be a trailing zero
}


//////////////////////////////////////////////////////////////////////////////
void test_4_bits(){
	std::stringstream bits;

	{
		BitOutput bito(bits);
		bito.output_bit(0);
		bito.output_bit(1);
		bito.output_bit(0);
		bito.output_bit(1);
	}

	BitInput biti(bits);
	assert(biti.input_bit() == false);
	assert(biti.input_bit() == true);
	assert(biti.input_bit() == false);
	assert(biti.input_bit() == true);
	assert(biti.input_bit() == false);  // Should just be a trailing zero
}

//////////////////////////////////////////////////////////////////////////////
void test_8_bits(){
	std::stringstream bits0, bits1;

	{
		BitOutput bito0(bits0);
		BitOutput bito1(bits1);
		for (int i = 0; i < 8; ++i) {
			bito0.output_bit(0);
		}
		for (int i = 0; i < 8; ++i) {
			bito1.output_bit(1);
		}
	}

	BitInput biti0(bits0);
	BitInput biti1(bits1);
	for (int i = 0; i < 8; ++i) {
		assert(!biti0.input_bit());
	}
	biti0.input_bit();
	assert(bits0.eof()); // Should be no trailing zeros this time!

	for (int i = 0; i < 8; ++i) {
		assert(biti1.input_bit());
	}
}

//////////////////////////////////////////////////////////////////////////////
void test_9_bits(){
	std::stringstream bits0, bits1;

	{
		BitOutput bito1(bits1);
		for (int i = 0; i < 9; ++i) {
			bito1.output_bit(1);
		}
	}

	BitInput biti1(bits1);
	for (int i = 0; i < 9; ++i) {
		assert(biti1.input_bit());
	}
	assert(!biti1.input_bit());
}

//////////////////////////////////////////////////////////////////////////////
void test_16_bits(){
	std::stringstream bits;

	{
		BitOutput bito(bits);
		for (int i = 0; i < 16; ++i) {
			bito.output_bit(i % 2);
		}
	}

	BitInput biti(bits);
	for (int i = 0; i < 16; ++i) {
		assert(biti.input_bit() == i % 2);
	}

	biti.input_bit();
	assert(bits.eof()); // Should be no trailing zeros this time!
}

//////////////////////////////////////////////////////////////////////////////
void test_100_bits(){
	std::stringstream bits;

	{
		BitOutput bito(bits);
		for (int i = 0; i < 100; ++i) {
			bito.output_bit(!(i % 2));
		}
	}

	BitInput biti(bits);
	for (int i = 0; i < 100; ++i) {
		assert(biti.input_bit() != i % 2);
	}
	assert(!biti.input_bit());
}


//////////////////////////////////////////////////////////////////////////////
int main(){
	test_1_bit();
	test_4_bits();
	test_8_bits();
	test_9_bits();
	test_16_bits();
	test_100_bits();

	return 0;
}


/*bitio.cc with debuc std::cout lines for debugging:

#include "bitio.hh"
#include <cassert>
#include <climits>
#include <iostream>

BitInput::BitInput(std::istream& is){
	//Construct a stream object of the appropriate class (is it input or output? File or network? etc.)
	//Associate the object with the appropriate physical device
	is_ = &is;			//input stream in use
	symbol_ = is_->get();		//the symbol bits are being read from
	//was originally 0
	bit_offset_ = 1;	//determines which 2s place bits are read from
}

//bits are read per character left to right, most to least significant
bool BitInput::input_bit(){
	std::cout << "####intput_bit() called\n";
	assert(is_);	//ensures is_ != nullptr
	//if previous character is finished being read, go to next char
	if(bit_offset_ > CHAR_BIT){
		std::cout << "character finished being input\n";
		symbol_ = is_->get();	//read next character to convert to bits from istream
		bit_offset_ = 1;		//reset bit offset
	}
	std::cout << "shifting 1 left " << int(CHAR_BIT - bit_offset_) << " times to become mask\n";
	std::cout << "mask should be " << int(1 << (CHAR_BIT - bit_offset_)) << "\n";
	char_int_t mask = 1 << (CHAR_BIT - bit_offset_); //mask is an int with a 1 in its (CHAR_BIT - place_) place when in binary form
	std::cout << "mask: " << int(mask) << ", symbol: " << int(symbol_) << "\n";
	bool bit = symbol_ & mask;	//true if symbol has 1 in ^ place in binary form, false if 0
	std::cout << "bit: " << bit << "\n\n";
	++bit_offset_;
	return bit;
}

BitOutput::BitOutput(std::ostream& os){
	os_ = &os;			//output stream in use
	symbol_ = 0;		//idk lol
	bits_written_ = 0;	//how many bits have been written into current character
}


//ensures final symbol is finished, and if it's not... finish it anyways
BitOutput::~BitOutput(){
	std::cout << "####destructor called\n";
	if(symbol_ != 0){	//if current symbol is unfinished
		std::cout << "symbol: " << int(symbol_) << ", bits written: " << bits_written_ << "\n";
		std::cout << "symbol will become " << int(symbol_ << (CHAR_BIT - bits_written_)) << "\n\n";
		symbol_ = symbol_ << (CHAR_BIT - bits_written_); //adds trailing zeros? not sure abt that instruction, shouldve asked for clearer instructions but i accidentally slept through the second lab lol
		os_->put(symbol_);	//flushes current bits to ostream? not sure what "flush" instruction meant, see comment ^
	}
}

//bits are written per character left to right, most to least significant
void BitOutput::output_bit(bool bit){
	std::cout << "####output_bit called\n";
	std::cout << "symbol: " << int(symbol_) << ", bit: " << bit << ", bits written: " << bits_written_ << "\n";
	assert(os_);	//os != nullptr
	symbol_ = symbol_ << 1;		//make space for new bit
	std::cout << "symbol_ << 1: " << int(symbol_) << "\n";
	symbol_ = symbol_ | bit;	//add new bit to symbol_ in the 1s place
	std::cout << "symbol_ | bit: " << int(symbol_);
	++bits_written_;

	if(bits_written_ == CHAR_BIT){	//if entire character is written
		std::cout << "character finished\n";
		std::cout << "writing symbol: " << int(symbol_);
		os_->put(symbol_);	//write symbol to ostream
		bits_written_ = 0;	//reset bit offset
		symbol_ = 0;		//reset symbol
	}
	std::cout << "\n\n";
}
*/