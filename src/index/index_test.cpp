#include "test_index_builder.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>
#include <fstream>

int main() {
    //compile test
    //IndexBuilder<27, 10> indexBuilder("/Users/grantreszczyk/workspace/CLASS/EECS398/Search_Engine/tmp");
	string filename = "";
	ifstream f;
	f.open(filename);
	//masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(MasterIndexData), PROT_READ | PROT_WRITE, MAP_PRIVATE, file, 0);
	// I don't know how to mmap this :(
	char* beginning_of_file;
	std::vector<std::vector<uint64_t>> all; 
	std::vector<std::string> words; 
	std::vector<std::pair<size_t,uint64_t>> EOD_posting_list;
	trans_file_to_offsets(beginning_of_file, all, words, EOD_posting_list)	
	size_t count = 0;
	for(size_t i = 0; i < all.size(); ++i){
		for(size_t j = 0; j < all[i].size(); ++j){
			++count;
		}
	}
	std::vector<std::string> original(count, "");
	reconstruct(all, words, original);
	print_recon(original);
}
