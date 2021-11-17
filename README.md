# csci-221-hw-08

Makefile changes
================

Makefile was mostly the same, save for:
- htree.o had to be added to a few of the test function rules
- clean was modified to remove all .comp and .plaintext files
- CXX had to be set to g++, not g++-8



huffman.hh & huffman.cc
=======================

Huffman.cc is fairly well doccumented by comments, but here's an overview:

Huffman essentially follows the steps given on Moodle. It has two private data members:
 - freq_ is the frequency table of the symbols, where the ascii number of a symbol is its place in the vector
 - decoding_tree_ is the tree which is used to decode bits, see decode()\
It also has one private member function:
 - planter() is a very cleverly named function which creates an HTree used to decode, see decode()

Huffman's functions are:
 - the constructor, which sets the size of the frequency table and initializes it to the proper values, as well as setting decoding_tree_ to nullptr
    > I used to use `std::fill(freq_.begin(), freq_.end(), 0)`, which took me about 36 hours to figure out was causing me problems and is the primary reason this assignment was late
 - planter, which creates a tree of all symbols based on the frequency table according to the steps given on moodle. 
 - encode(), which encodes a symbol into a vector of bits according to the steps given on moodle and updates the frequency of the symbol it decodes
 - decode, which is used to decode symbols one bit at a time. it constructs a tree based on the current frequency of symbols, then goes down the tree according to the bits it is given until it reaches a leaf which holds a symbol, returns the symbol, then updates the frequency table and resets the decoding tree. if it does not find a symbol, it returns a negative key.

Huffman.cc has a debug version commented out below it, where it prints various things which were sometimes helpful in debugging it. The cout statements are commented out, as it was last used in the final build.




bitio.hh & bitio.cc
=======================

bitio.cc is also rather well documented by comments, but here's an overview as well:

In bitio.hh,
- char_int_t is defined as uint8_t to be efficient with space and make for easy editing if CHAR_BIT changes, as char_int_t is used for the size of symbols that are being decoded throughout bitio.hh and bitio.cc in both BitInput and BitOutput. Ideally this would dynamically change based off of what BIT_SIZE is and would be equal to something like `uint(CHAR_BIT)_t`, where `(CHAR_BIT)` is replaced by whatever `CHAR_BIT` is.
- there are 3 private data members for both BitInput and BitOutput which are described in the constructors of their classes

### BitInput in bitio.cc:
BitInput():
	- The constructor sets up the istream to be what is passed to it, sets symbol_ to begin with the first symbol of the istream, and sets the bit offset to 1, meaning the first bit will be written in the 8s place
input_bit():
	- This function takes a symbol from the istream and returns a bit of it every time it is called, then moves to the next symbol once a symbol is finished. The process is something like this:
	1. make sure is_ can be read from
	2. if the symbol is finished being read (determined by `bit_offset_ > CHAR_BIT`, i.e. bits would be being read from a binary place larger than the size of the symbols), move to the next symbol in instream
	3. create a "mask" with a 1 in the binary place where you want to read bits from and all other bits are 0s, then compare with symbol using bitwise and to see if symbol has a 1 there
	4. increase the bit offset so next time, the next bit is read
	5. return the bit in the placce being checked
	It's important to note that this function reads bits starting from the left side of the symbol's binary representation.

### BitOutput in bitio.cc
BitOutput():
	- The constructor initializes the output stream symbols will be written to, starts the symbol which it will begin writing bits into as all zeroes, and initializes the amount of bits written into the current symbol it's working on as zero.\
~BitOutput():
	- The desctructor ensures that the last symbol being written into is finished and therefore written to the ostream. If not, it "finishes" the rest of the bits with trailing zeroes and writies it to the ostream. It's important to note the zeroes are trailing as BitOutput writes bits from left to right per character and BitInput writes bites left to right per character as well.
output_bit():\
	- The function takes a bit at a time as arguments and creates a symbol once it has a CHAR_BIT amount of them, then writes it to the ostream. It does this by:\
	1. make sure os_ can be written to
	2. shift all currently written bits left once so that there is space for the new bit in the 2s place
	3. add the new bit to the 2s place using bitwise or
	3. add one to the count of bits written to the current symbol
	4. if all `CHAR_BIT` number of bits have been written (here it would be 8), then the symbol is finished, so write it to the ostream and reset both the counter of bits written and the current symbol to 0



encoder.cc & decoder.cc
=======================

### encoder.cc
The encoder takes command line arguments as names of files. If one or more files are passed, for each of the files, the encoder:
	1. sets up the istream to read symbols from the raw file, and the ostream to write symbols to the compressed file
	2. Creates a Huffman object for encoding and a BitOutput object for writing bits to the ofstream object
	3. Sets the current symbol to 0; this ensures that 0 doesn't somehow get initialized as the EOF character
	4. Define a vector of bits
	5. Until the current symbol decoded is the EOF character,
		a) set the variable symbol to be the next symbol to be encoded
		b) is there's no more symbols to encode, set symbol to be the EOF character
		c) encode the symbol into bits using the Huffman object, and for every bit in that, use the BitOutput object to output the compressed symbols to the compressed file
	6. Close the iostreams

### decoder.cc
This has my favorite life of code from all semester, line 28. It is awful, but I like it too much to change it.\
The decoder takes command line arguments in the same manner as the encoder, and for each:
	1. sets up the istream to read compressed symbols from the compressed file, and the ostream to write symbols to the plaintext file
	2. Creates a huffman object for decoding and a BitInput object for inputting bits
	3. Initializes symbol as 0 so that it can't be initialized as garbage which happens to be EOF character
	4. Until the symbol decoded is the EOF character,
		a) use the huffman object to decode the bits from the BitInput object which it gets from the istream, until either the istream is over or the huffman obect decodes a full uncompressed symbol (which has a key greater than 0)
		b) If the symbol is the EOF symbol, set the symbol to be the EOF symbol. this could alternatively be `break` and the while loop could be `while(true)`, but this is closer to my intentions and is more readable.
		c) If the symbol isn't the EOF symbol, write it to the plaintext file
	5. Close the iostreams



Compression Test Results
========================

file name	| raw file size	| compressed file size	| decompressed matches raw file size?\
cats1		| 491 b			| 289 b					| yes\
cats2		| 1,866 b		| 534 b					| yes\
long_file	| 7,924 b		| 4,219 b				| yes\



other
=====

I recieved an extension from Eitan and his solution to this homework has not been shown in class yet.

While I did do this largely on my own, I did have some help from Rebecca. I definitely wouldn't say we were partners, but she did help point me in the direction of what to do for bitio especially, as I accidentally slept through the second lab and couldn't ask what to do then.

I initially submitted the github link before I was finished, thinking it would be good to make sure I didn't forget to submit it after committing all the files. I then ended up forgetting to commit the files until after the extended deadline. Rebecca and I still haven't seen Eitan's solution; we assume he will be showing that in the next class?