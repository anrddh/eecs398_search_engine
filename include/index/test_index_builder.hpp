#include "fb/indexEntry.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
using namespace fb;


// TO DO: Fill this in
constexpr int SKIP_TABLE_BYTES = (1 << 4) * 2 * sizeof(unsigned int);

// read a single posting list and add all the nums to vector
void read_posting_list(char* current, std::vector<uint32_t> &posting_list){
	while(!fb::is_word_sentinel(current)){
      uint64_t num = 0;
		current = read_word_post(current, num);
      posting_list.push_back(num);
	}
}

// read EOD posting list and add values to vector
char * read_EOD_posting_list(char* current, std::vector<std::pair<uint32_t,uint32_t>> &EOD_posting_list){
	size_t delta = 0;
	uint32_t url_id = 0;
   ++current; 
   current += (2 * sizeof(unsigned int));
   current += SKIP_TABLE_BYTES;

	while(true){
		current = read_document_post(current, delta, url_id);
		if(delta == 0){
			break;
		}else{
			EOD_posting_list.push_back(std::pair<size_t,uint64_t>(delta, url_id));
		}
	}

   return current;
}

// given pointer to start of file, creates a vector of vectors
// where each vector is a posting list containing offsets
void trans_file_to_offsets(char* start, std::vector<std::vector<uint32_t>> &all, std::vector<std::string> &words, std::vector<std::pair<uint32_t,uint32_t>> &EOD_posting_list)
   {
   char* current = start;
   current += (2 * sizeof(unsigned int));
   
	unsigned int table_size = *((unsigned int *) current);
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
         while(*(current++) != '\0')
            ;
         current += (2 * sizeof(unsigned int));
         current += SKIP_TABLE_BYTES;
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

//basic print function
void print_recon(std::vector<std::string> &original){
	for(std::string word : original){
		std::cout << word << "\n";
	}
   std::cout << std::endl;
}