#include "index/test_index_builder.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char* argv[]) {
	if(argc != 2){
		perror("USAGE: ./exe INDEX_FILE");
		exit(EXIT_FAILURE);
	}
	const char* FILEPATH = argv[1];
	int fd = open(FILEPATH, O_RDWR);
   	//std::cout << "fd: " << fd << std::endl;
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
    }
	struct stat sb;
	int res = fstat(fd, &sb);
   	//std::cout << "res: " << res << std::endl;
   	//std::cout << sb.st_size << std::endl;
	char* beginning_of_file = (char *)mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
	if (beginning_of_file == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
   	}

	std::vector<std::vector<uint32_t>> all; 
	std::vector<std::string> words; 
	std::vector<std::pair<uint32_t,uint32_t>> EOD_posting_list;
	trans_file_to_offsets(beginning_of_file, all, words, EOD_posting_list);	
	// std::cout << "all: " <<all.size() << std::endl;
	// if(!all.empty()){
	// 	std::cout << "all[0]:" << all[0].size() << std::endl;
	// }
	// for(int i = 0; i < words.size(); ++i){
	// 	std::cout << words[i] << std::endl;
	// }
	size_t count = 0;
	for(size_t i = 0; i < all.size(); ++i){
		for(size_t j = 0; j < all[i].size(); ++j){
			++count;
		}
	}

	std::vector<std::string> original(count + EOD_posting_list.size() + 1, "");
	reconstruct(all, words, original);
	//std::cout << "original: " << original.size() << std::endl;
	print_recon(original);
	//std::cout << "word count: " << words.size() << std::endl;
	//std::cout << "DONE" << std::endl;
}