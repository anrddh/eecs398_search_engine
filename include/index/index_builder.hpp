#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "fb/string.hpp"
#include "fb/memory.hpp"
#include "fb/functional.hpp"
#include "fb/mutex.hpp"
#include "fb/thread.hpp"
#include "fb/unordered_set.hpp"

#include "index/index_chunk_builder.hpp"
#include "index/index_helpers.hpp" 
#include "index/index_data_structures.hpp"

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */


template<int NUM_SKIP_TABLE_BITS>
class IndexBuilder {
public:
   // root must contain a trailing '/'
   IndexBuilder(fb::String path) : root(path) 
      { 
      int file;
      if((file = open((path + "master").data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH)) < 0)
         {
         // abort or something maybe with error message
         exit(1);
         }
      ftruncate(file, sizeof(masterIndexData));
      masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(masterIndexData), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, file, 0);

      close(file);
      
      tokenCount = 1;
      }
   
   void build_chunk(uint64_t* start_of_file, int chunk) {
      // move past first 16 bytes
      // the page headers end where the first page starts
      uint64_t* current_doc_offset = start_of_file + 2;
      uint64_t* current_des_offset = start_of_file + 3;
      unsigned int num_pages = start_of_file[1];
      uint64_t* end_page_headers = start_of_file + 2 + (num_pages * 3);
      uint64_t doc_num = 0;
      uint8_t * start = (uint8_t *) start_of_file;
      while(current_doc_offset != end_page_headers){
         build_single_doc(start + *current_doc_offset, start + *current_des_offset, doc_num);
         current_doc_offset += 3;
         current_des_offset += 3;
         ++doc_num;
      }
      flushToDisk(chunk);
   }
private:
   // reads a single space terminated word
   // returns pointer to the beginning of the next word
   // increments tokenCount
   char* read_word(char* &word_begin, fb::String &word){
      word = "";
      while(*word_begin != ' ' && *word_begin != '\0'){
         word = word + *word_begin;
         ++word_begin;
      }
      ++tokenCount;
      // check to see if we are at the end of a document
      if(*word_begin == '\0'){
         return word_begin;
      }
      // otherwise move past the space and return the start
      // of the next word
      ++word_begin;
      return word_begin;
   }

   void build_single_doc(uint8_t* doc_start, uint8_t* des_start, uint64_t docId){
      fb::UnorderedSet<fb::String> unique_words;
      fb::String word;
      uint8_t word_info;
      char* current_word = (char *)doc_start;
      uint8_t* current_des = des_start;
      while(*current_word != '\0'){
         current_word = read_word(current_word, word);
         unique_words.insert(word);
         word_info = *current_des;
         AbsoluteWordInfo absWord = {tokenCount, word_info};
         wordPositions[word].pushBack(absWord);
      }
      for(fb::String thing : unique_words){
         ++wordDocCounts[thing];
      }
      // increment for EOD
      ++tokenCount;
      DocIdInfo doc_info = {tokenCount, docId};
      documents.pushBack(doc_info);
   }
   
   void flushToDisk(int chunk) 
      {
      fb::String filename = (root + "Index" + fb::toString(chunk));
      // change this to use atomics
      ++masterIndexData->numIndexes;

      IndexChunkBuilder<fb::Hash<fb::String>, NUM_SKIP_TABLE_BITS> indexChunkBuilder(filename, wordPositions.bucket_count(), documents, tokenCount);

      for(auto iter = wordPositions.begin(); iter != wordPositions.end(); ++iter)
         {
         indexChunkBuilder.addWord(iter.key(), *iter, wordDocCounts[iter.key()]);
         }

      tokenCount = 1;
      }

   // folder for each index chunk, store root directory
   fb::String root;
   
   // some meta data about the index chunks in this index
   MasterIndexData * masterIndexData;
   fb::Mutex masterIndexDataLock;

   // the actual map that stores the positions of the words, is a unique pointer so that we can pass this ownership to a thread.
   fb::UnorderedMap<fb::String, fb::Vector<AbsoluteWordInfo>> wordPositions;
   // this is the number of documents a word appears in
   fb::UnorderedMap<fb::String, unsigned int> wordDocCounts;
  	fb::Mutex wordPositionsLock;
   fb::Vector<DocIdInfo> documents;

   // each chunk keeps track of its own word count
   unsigned int tokenCount;
  	
};



