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

#define FILEPATH "PageStore0"

char* read_word(char* &word_begin, std::string &word){
	word = "";
	while(*word_begin != ' ' && *word_begin != '\0'){
		word = word + *word_begin;
		++word_begin;
	}
	// check to see if we are at the end of a document
	if(*word_begin == '\0'){
		return word_begin;
	}
	// otherwise move past the space and return the start
	// of the next word
	++word_begin;
	return word_begin;
}

void print_single_doc(uint8_t* doc_start){
	std::string word;
	char* current_word = (char*) doc_start;
	while(*current_word != '\0'){
		current_word = read_word(current_word, word);
		std::cout << word << "\n";
	}
}

void print_page_store_file(uint64_t* start_of_file){
	std::cout << "\n";
	uint64_t* current_doc_offset = start_of_file + 2;
	unsigned int num_pages = start_of_file[1];
	uint64_t* end_page_headers = start_of_file + 2 + (num_pages * 3);
	uint8_t * start = (uint8_t *) start_of_file;
	while(current_doc_offset != end_page_headers){
		print_single_doc(start + *current_doc_offset);
		std::cout << "\n";
		current_doc_offset += 3;
    }
}

int main(){
	int fd = open(FILEPATH, O_RDWR);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	int res = fstat(fd, &sb);
	uint64_t* beginning_of_file = (uint64_t *)mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
	if (beginning_of_file == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	print_page_store_file(beginning_of_file);
}