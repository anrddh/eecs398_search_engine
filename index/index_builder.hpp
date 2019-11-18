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

#include "index_chunk_builder.hpp"
#include "index_helpers.hpp"
#include "index_data_structures.hpp"

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */

//
// constexptr int MAX_BITS_PER_CHUNK = 28;
// constexptr int TOKEN_THRESHOLD = 2 << MAX_BITS_PER_CHUNK; //134,217,728; // 2^27

template<int MAX_BITS_PER_CHUNK, int NUM_SKIP_TABLE_BITS>
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
   //takes in vector of IndexWords, maps words to indices in vector/flags
   bool addDocument(unsigned int docId, fb::Vector<IndexWord> &words) {
      wordPositionsLock.lock();
      for(auto word : words)
      {
         AbsoluteWordInfo info = {tokenCount, word.type_flags}; 
         wordPositions[word.word].pushBack(info);
         ++wordDocCounts[word.word];
         ++tokenCount;
      }

      // what is emplace_back? emplace back forwards the arguments you pass it to the constructor of type T. It constructs an object of type T in place which is why it is useufl
      documents.emplaceBack(tokenCount, docId);
      ++tokenCount;

      constexpr int MAX_TOKEN_COUNT = 2 << MAX_BITS_PER_CHUNK;
      if(tokenCount > MAX_TOKEN_COUNT) {
         flushToDisk();
      }
      wordPositionsLock.unlock();
   }

private:
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

      IndexChunkBuilder<fb::Hash<fb::String>, NUM_SKIP_TABLE_BITS> indexChunkBuilder(input.filename, input.map.bucket_count());

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
   fb::UnorderedMap<fb::String, unsigned int> wordDocCounts;
  	fb::Mutex wordPositionsLock;
   fb::Vector<DocIdInfo> documents;
    
   // each chunk keeps track of its own word count
   int tokenCount;
  	
};