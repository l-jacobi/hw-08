/*
 * A pair of simple classes tu perform stream I/O on individual bits.
 */
#include <iostream>

#pragma once

using char_int_t = uint8_t;	//for consistency across symbol_ in this file and mask in bitio.cc
							//should be changed if CHAR_BIT != 8, not sure how to code that in automatically

// BitInput: Read a single bit at a time from an input stream.
// Before reading any bits, ensure your input stream still has valid inputs.
class BitInput {
	public:
		// Construct with an input stream
		BitInput(std::istream& is);

		BitInput(const BitInput&) = default;
		BitInput(BitInput&&) = default;
		BitInput& operator=(const BitInput&) = default;
		BitInput& operator=(BitInput&&) = default;

		// Read a single bit (or trailing zero)
		// Allowed to crash or throw an exception if called past end-of-file.
		// Returns bit based on what's in istream
		bool input_bit();
	private:
		std::istream* is_; //can this be of type std::istream and be passed a reference to is? test this later maybe
		char_int_t symbol_;	
		int bit_offset_;
};

// BitOutput: Write a single bit at a time to an output stream
// Make sure all bits are written out by the time the destructor is done.
class BitOutput {
	public:
		// Construct with an output stream
		BitOutput(std::ostream& os);

		// Flushes out any remaining output bits and trailing zeros, if any:
		~BitOutput();

		BitOutput(const BitOutput&) = default;
		BitOutput(BitOutput&&) = default;
		BitOutput& operator=(const BitOutput&) = default;
		BitOutput& operator=(BitOutput&&) = default;

		// Output a single bit (buffered); writes to ostream obj
		void output_bit(bool bit);
	private:
		std::ostream* os_;
		char_int_t symbol_;
		int bits_written_;
};
