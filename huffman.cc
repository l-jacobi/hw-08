#include "huffman.hh"
//#include "hforest.hh"
//#include "htree.hh"
#include <algorithm>
#include <memory>
#include <cassert>

//#include <iostream>
//all uses of cout are for debugging and should be commented out

using D = HTree::Direction;

//initialize frequency table to all 0s except eof at 1
Huffman::Huffman(){ 
	freq_.reserve(ALPHABET_SIZE);
	//std::fill(freq_.begin(), freq_.end(), 0);
	//this ^ totally fucks up the frequency table when decoding and should never be used, wtf
	//that line genuinely took me 36 hours to figure out what the problem was. This assignment would be turned in on time if I never used it.
	for(int symbol = 0; symbol < ALPHABET_SIZE; symbol++){
		freq_[symbol] = 0;
	}
	freq_[HEOF] = 1;
	decoding_tree_ = nullptr;
}

HForest::tree_t Huffman::planter(){
	//initialize forest
	std::shared_ptr<HForest> hforest = std::make_shared<HForest>();
	for(int symbol = 0; symbol < Huffman::ALPHABET_SIZE; ++symbol){
		hforest->add_tree(std::make_shared<HTree>(symbol, freq_[symbol]));
	}
	//connect htrees
	int fake_key = -1;
	while(hforest->size() != 1){
		--fake_key;
		//hforest->add_tree(std::make_shared<HTree>(keys, value, hforest->pop_top(), hforest->pop_top()));
		HForest::tree_t l_tree = hforest->pop_top();
		HForest::tree_t r_tree = hforest->pop_top();
		hforest->add_tree(std::make_shared<HTree>(fake_key, l_tree->get_value() + r_tree->get_value(), l_tree, r_tree));
	}
	assert(hforest->size() == 1);
	HForest::tree_t htree = hforest->pop_top();
	return htree;
}

Huffman::bits_t Huffman::encode(int symbol){
	//build an htree from scratch using the current frequency table
		HForest::tree_t htree = planter();
	//Find the path to c in the Huffman tree from the root
		HTree::possible_path_t path = htree->path_to(symbol);
	//Convert the path of LEFTs and RIGHTs to zeros and ones, which will be returned as a sequence of bits (type bits_t).
		bits_t bits;
		for(D dir : *path){	//for(D dir : *(hforest->pop_top())->path_to(symbol)){	//beautiful line of code i can't use because the compiler doesn't like it for reasons i don't have time to figure out. probably has to do with its deletion
			switch(dir){
				case D::LEFT:
					bits.push_back(0);
					break;
				case D::RIGHT:
					bits.push_back(1);
					break;
				default:
					assert(false);
			}
		}
	//update frequency table
		++freq_[symbol];
	return bits;
}

int Huffman::decode(bool bit){
	//If this is the first bit in a sequence (Huffman tree not yet computed), build a Huffman tree given the existing frequency table, and start from the root node.
		if(!decoding_tree_){
			decoding_tree_ = planter();
		}
	//Compute the next node as either the left or right child of the current node, depending on the bit value
		if(!bit){
			decoding_tree_ = decoding_tree_->get_child(D::LEFT);
		}else{
			decoding_tree_ = decoding_tree_->get_child(D::RIGHT);
		}
	//If this is an intermediate node, return a negative number for a symbol. If leaf, return symbol, reset decoding tree, set freq table
		if(!(decoding_tree_->get_child(D::LEFT)) && !(decoding_tree_->get_child(D::RIGHT))){
			int symbol = decoding_tree_->get_key();
			++freq_[symbol];
			decoding_tree_ = nullptr;
			return symbol;
		}else{
			return decoding_tree_->get_key();
		}
}

/*Debug version with cout statements commented out:

#include "huffman.hh"
//#include "hforest.hh"
//#include "htree.hh"
#include <algorithm>
#include <memory>
#include <cassert>

//#include <iostream>
//all uses of cout are for debugging and should be commented out

using D = HTree::Direction;

//initialize frequency table to all 0s except eof at 1
Huffman::Huffman(){ 
	freq_.reserve(ALPHABET_SIZE);
	//std::fill(freq_.begin(), freq_.end(), 0);
	//this ^ totally fucks up the frequency table when decoding and should never be used, wtf
	//that line genuinely took me 36 hours to figure out what the problem was. This assignment would be turned in on time if I never used it.
	for(int symbol = 0; symbol < ALPHABET_SIZE; symbol++){
		freq_[symbol] = 0;
	}
	freq_[HEOF] = 1;
	decoding_tree_ = nullptr;
}

HForest::tree_t Huffman::planter(){
	//initialize forest
	std::shared_ptr<HForest> hforest = std::make_shared<HForest>();
/ *		std::cout << "frequecy table tree is about to be made with: " << std::endl;
	for(int symbol = 0; symbol < Huffman::ALPHABET_SIZE; ++symbol){
		if(freq_[symbol])
			std::cout << symbol << " | " << freq_[symbol] << std::endl;
	} * /
	for(int symbol = 0; symbol < Huffman::ALPHABET_SIZE; ++symbol){
		hforest->add_tree(std::make_shared<HTree>(symbol, freq_[symbol]));
	}
	//connect htrees
	int fake_key = -1;
	while(hforest->size() != 1){
		--fake_key;
		//hforest->add_tree(std::make_shared<HTree>(keys, value, hforest->pop_top(), hforest->pop_top()));
		HForest::tree_t l_tree = hforest->pop_top();
		HForest::tree_t r_tree = hforest->pop_top();
/ *		if(value_sum || l_tree->get_value() || r_tree->get_value()){
			std::cout << "L: " << l_tree->get_key() << ", " << l_tree->get_value() <<
					" | R: " << r_tree->get_key() << ", " << r_tree->get_value() << std::endl;
			std::cout << "new value " << value_sum << std::endl << std::endl;
		}* /
		hforest->add_tree(std::make_shared<HTree>(fake_key, l_tree->get_value() + r_tree->get_value(), l_tree, r_tree));
	}
	assert(hforest->size() == 1);
	HForest::tree_t htree = hforest->pop_top();
	return htree;
}

Huffman::bits_t Huffman::encode(int symbol){
//	std::cout << "encoding " << symbol << std::endl << std::endl;
	//build an htree from scratch using the current frequency table
		HForest::tree_t htree = planter();
	//Find the path to c in the Huffman tree from the root
		HTree::possible_path_t path = htree->path_to(symbol);
	//Convert the path of LEFTs and RIGHTs to zeros and ones, which will be returned as a sequence of bits (type bits_t).
		bits_t bits;
		for(D dir : *path){	//for(D dir : *(hforest->pop_top())->path_to(symbol)){	//beautiful line of code i can't use because the compiler doesn't like it for reasons i don't have time to figure out. probably has to do with its deletion
			switch(dir){
				case D::LEFT:
					bits.push_back(0);
					break;
				case D::RIGHT:
					bits.push_back(1);
					break;
				default:
					assert(false);
			}
		}
	//update frequency table
		++freq_[symbol];
	return bits;
}

int Huffman::decode(bool bit){
	//If this is the first bit in a sequence (Huffman tree not yet computed), build a Huffman tree given the existing frequency table, and start from the root node.
		if(!decoding_tree_){
			decoding_tree_ = planter();
		}
//	std::cout << "key: " << decoding_tree_->get_key() << ", value: " << decoding_tree_->get_value() << std::endl <<
//				"L child: " << (decoding_tree_->get_child(D::LEFT)) << ", R child: " << (decoding_tree_->get_child(D::RIGHT)) << std::endl <<
//				"L: " << (decoding_tree_->get_child(D::LEFT))->get_key() << ", " << (decoding_tree_->get_child(D::LEFT))->get_value() << " | R: " << (decoding_tree_->get_child(D::RIGHT))->get_key() << ", " << (decoding_tree_->get_child(D::RIGHT))->get_value() << std::endl;

	//Compute the next node as either the left or right child of the current node, depending on the bit value
		if(!bit){
//			std::cout << "going left \n";
			decoding_tree_ = decoding_tree_->get_child(D::LEFT);
		}else{
//			std::cout << "going right \n";
			decoding_tree_ = decoding_tree_->get_child(D::RIGHT);
		}
	//If this is an intermediate node, return a negative number for a symbol. If leaf, return symbol, reset decoding tree, set freq table
//	std::cout << "key: " << decoding_tree_->get_key() << ", value: " << decoding_tree_->get_value() << std::endl <<
//				"L child: " << (decoding_tree_->get_child(D::LEFT)) << ", R child: " << (decoding_tree_->get_child(D::RIGHT)) << std::endl;// <<
//		//		"key at L: " << (decoding_tree_->get_child(D::LEFT))->get_key() << ", key at R: " << (decoding_tree_->get_child(D::RIGHT))->get_key() << std::endl;
		if(!(decoding_tree_->get_child(D::LEFT)) && !(decoding_tree_->get_child(D::RIGHT))){
			int symbol = decoding_tree_->get_key();
			++freq_[symbol];
			decoding_tree_ = nullptr;
			return symbol;
		}else{
			return decoding_tree_->get_key();
		}
//	return 90;
}


*/