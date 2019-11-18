#include "indexEntry.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
using namespace fb;

// file structure:
// 1) 4 bytes (size of the whole table)
// 2) 4 bytes per offset into the file
// 3) posting lists
    
// posting list structure:
// 1) word (null terminated) 
// 2) 4 bytes (num documents in this posting list)
// 3) 4 bytes (num occurences = length of posting list)
// 4) 4 bytes (abs word num of the last word in posting list) 
// 5) skip table
// 6) posts


// TO DO: Fill this in
constexpr int SKIP_TABLE_BYTES = 0;

// need to read a single posting list and add all the nums to vector
void trans_posting_list(char* current, std::vector<uint64_t> &all){
	uint64_t num = 0;
	uint8_t header = 0;
	while(true){
		current = read_number(current, num, header);
		if(num == 0){
			break;
		}else{
			all.push_back(num);
		}
	}
}

// given pointer to start of file, creates a vector of vectors
// where each vector is a posting list containing offsets
void trans_file_to_offsets(char* current, std::vector<std::vector<uint64_t>> &all, std::vector<std::string> &words){
	std::vector<uint64_t> posting_list;
	char* start = current;
	//cast to a 4 byte type
	(unsigned int*)(current);
	//read in 4 bytes
	unsigned int table_size = *current;
	//skip forward 4 bytes
	current += 1;
	//read in all the posting list offsets and store in a vector
	std::vector<unsigned int> posting_list_offsets;
	for(unsigned int i = 0; i < table_size; ++i){
		posting_list_offsets.push_back(*current);
		current += 1;
	}
	//places all the offsets in all the posting lists in vector
	for(unsigned int i = 0; i < table_size; ++i){
		//cast to 1 byte type
		(char*)(current);
		current = start + posting_list_offsets[i];
		std::string word = "";
		while(true){
			if(*current == '\0'){
				++current;
				break;
			}
			word = word + *current;
		}
		words.push_back(word);
		(unsigned int*)(current);
		//move past metadata
		current += 3;
		//cast to a 1 byte type
		(char*)(current);
		trans_posting_list(current, posting_list);
		all.push_back(posting_list);
	}
}

//given words and all the offsets, reconstruct original vector
void reconstruct(std::vector<std::vector<uint64_t>> &all, std::vector<std::string> &words, std::vector<std::string> &original){
	//index i keeps track of word
	for(int i = 0; i < words.size(); ++i){
		std::string current_word = words[i];
		int current_index = all[i][0];
		//index j keeps track of post in a posting list for a word
		original[current_index] = current_word;
		for(int j = 1; j < all[i].size(); ++j){
			current_index = current_index + all[i][j];
			original[current_index] = current_word;
		}	
	}
}

//basic print function
void print_recon(std::vector<std::string> &original){
	for(std::string word : original){
		std::cout << word << " ";
	}
}