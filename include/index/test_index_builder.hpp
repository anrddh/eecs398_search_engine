#include "fb/indexEntry.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace fb;

//#include "index_helpers.hpp"
#include "index_builder.hpp"


// read a single posting list and add all the nums to vector
void read_posting_list(const char* current, std::vector<uint32_t> &posting_list){
	while(!fb::is_word_sentinel(current)){
		uint32_t num = -1;
		current = read_word_post(current, num);
		posting_list.push_back(num);
		//std::cout << "in the loop!" << std::endl;
	}
}

// read EOD posting list and add values to vector
const char * read_EOD_posting_list(const char* current, std::vector<std::pair<uint32_t,uint32_t>> &EOD_posting_list){
	uint32_t delta = 0;
	uint32_t url_id = 0;
	++current; 
	current += (2 * sizeof(unsigned int));
	int NUM_SKIP_TABLE_BITS = *((unsigned int *) current);
	//std::cout << "EOD skip table bits: " << NUM_SKIP_TABLE_BITS << std::endl;
	current += getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);

	while(true){
		current = read_document_post(current, delta, url_id);
		if(delta == 0){
			break;
		}else{
			EOD_posting_list.push_back(std::pair<uint32_t,uint32_t>(delta, url_id));
		}
	}

	return current;
}

// given pointer to start of file, creates a vector of vectors
// where each vector is a posting list containing offsets
void trans_file_to_offsets(const char* start, std::vector<std::vector<uint32_t>> &all, std::vector<std::string> &words, std::vector<std::pair<uint32_t,uint32_t>> &EOD_posting_list)
   {
	const char* current = start;
	current += (2 * sizeof(unsigned int));

	unsigned int table_size = *((unsigned int *) current);
	//std::cout << "table size: " << table_size << std::endl;
	current += sizeof(unsigned int);

	std::vector<unsigned int> posting_list_offsets;
	for(unsigned int i = 0; i < table_size; ++i){
		posting_list_offsets.push_back(*((unsigned int *) current));
		current += sizeof(unsigned int);
	}

	current = read_EOD_posting_list(current, EOD_posting_list);
	for(unsigned int i = 0; i < table_size; ++i)
		{
		if(posting_list_offsets[i])
			{
			current = start + posting_list_offsets[i];
			words.emplace_back(current);
			while(true){
	         	if(*current == '\0'){
	         		++current;
	         		break;
	         	}else{
	         		++current;	
	         	}
			}
	        
			current += (2 * sizeof(unsigned int));
			int NUM_SKIP_TABLE_BITS = *((int *) current);
			current += getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);
			//std::cout << words.back() << ":" << NUM_SKIP_TABLE_BITS << std::endl;
			std::vector<uint32_t> posting_list;
			read_posting_list(current, posting_list);
			all.push_back(posting_list);
			}
		}
   }

//given words and all the offsets, reconstruct original vector
void reconstruct(std::vector<std::vector<uint32_t>> &all, std::vector<std::string> &words, std::vector<std::string> &original){
	//index i keeps track of word
	for(size_t i = 0; i < words.size(); ++i){
		std::string current_word = words[i];
		int current_index = 0;
		for(size_t j = 0; j < all[i].size(); ++j){
			current_index = current_index + all[i][j];
			original[current_index] = current_word;
		}	
	}
}

void print_stats(std::vector<std::vector<uint32_t>> &all, std::vector<std::string> &words){
	std::ofstream output;
	output.open("posting_list_lengths.txt");
	std::ofstream output2;
	output2.open("posting_list_offsets.txt");
	size_t largest_offset = 0;
	for(size_t i = 0; i < all.size(); ++i){
		output << words[i] << ":" <<all[i].size() << "\n";
		size_t list_largest = *std::max_element(all[i].begin(), all[i].end());
		if(list_largest > largest_offset){
			largest_offset = list_largest;
		}
		for(size_t j = 0; j < all[i].size(); ++j){
			output2 << all[i][j] << "\n";
		}
	}
	std::cout << "largest offset: " << largest_offset << std::endl;
}

//basic print function
void print_recon(std::vector<std::string> &original){
	for(std::string word : original){
		std::cout << word << "\n";
	}
	std::cout << std::endl;
}