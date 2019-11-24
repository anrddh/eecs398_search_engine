#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "string.hpp"
#include "memory.hpp"
#include "functional.hpp"
#include "mutex.hpp"
#include "thread.hpp"

#include "index_chunk_builder.hpp"
#include "index_helpers.hpp"
#include "index_data_structures.hpp"

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */

//
// constexptr int MAX_BITS_PER_CHUNK = 28;
// constexptr int TOKEN_THRESHOLD = 2 << MAX_BITS_PER_CHUNK; //134,217,728; // 2^27

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
class IndexBuilder {
public:
   // root must contain a trailing '/'
   IndexBuilder(fb::String path) : root(path) { 
      int file;
      if((file = open((path + "master").data(), O_RDWR)) > 0) {
         masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(MasterIndexData), PROT_READ | PROT_WRITE, MAP_PRIVATE, file, 0);
      } else {
         // TODO: create file from scratch
      }
      close(file);
      
      tokenCount = 1;
   }
   
   void build_chunk(size_t* start_of_file){
      // move past first 16 bytes
      // the page headers end where the first page starts
      size_t* current_doc_offset = start_of_file + 2;
      size_t* current_des_offset = start_of_file + 3;
      //size_t* current_doc_id = start_of_file + 4;
      size_t* end_page_headers = start_of_file + *start_of_file;
      uint32_t doc_num = 0;
      while(current != end_page_headers){
         build_single_doc(start_of_file + *current_doc, start_of_file + *current_des, doc_num);
         current_doc_offset += 3;
         current_des_offset += 3;
         //current_doc_id += 3;
         ++doc_num;
      }
      flushToDisk();
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

   void build_single_doc(char* doc_start, uint8_t* des_start, uint32_t docId){
      fb::UnorderedSet<fb::String word> unique_words;
      fb::String word;
      uint8_t word_info;
      char* current_word = doc_start;
      uint8_t* current_des = des_start;
      while(*current_word != '\0'){
         current_word = read_word(current_word, word);
         unique_words.add(word);
         word_info = *current_des;
         AbsoluteWordInfo absWord = {tokenCount, word_info};
         wordPositions[word].pushBack(absWord);
      }
      for(thing : unique_words){
         ++wordDocCounts[thing];
      }
      // increment for EOD
      ++tokenCount;
      DocIdInfo doc_info = {tokenCount, docId};
      documents.pushBack(doc_info);
   }
   
   void flushToDisk() 
      {
      WriteToDiskInput * input = new WriteToDiskInput;
      input->filename = (root + "index_" + std::to_string(masterIndexData->numIndexes)).c_str();
      ++masterIndexData->numIndexes;
      fb::swap(input->map, wordPositions);
      fb::swap(input->wordDocCounts, wordDocCounts);

      fb::Thread worker(&writeToDisk, (void *) input);
      worker.detach();
      
      tokenCount = 1;
      }

   // create dictionary: maps words to offset in the file where posting list is
   static void * writeToDisk(void * arg) 
      {
      WriteToDiskInput input = *(WriteToDiskInput *)arg;

      IndexChunkBuilder<fb::Hash<fb::String>, NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK> indexChunkBuilder(input.filename, input.map.bucket_count());

      for(auto iter = input.map.begin(); iter != input.map.end(); ++iter)
         {
         indexChunkBuilder.addWord(iter.key(), *iter, input.wordDocCounts[iter.key()]);
         }

      delete (WriteToDiskInput *) arg;
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
   unsigned long tokenCount;
  	
};



