#pragma once

#include "posting_list_builder.hpp"
#include "index_data_structures.hpp"


template<typename Hash, int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
class IndexChunkBuilder {
public:
   IndexChunkBuilder(std::string filename, uint32_t capacity, const fb::Vector<DocIdInfo> &documents) 
      : tableSize(capacity), file(open(filename.data(), O_RDWR | O_CREAT)) 
      { 
      if(file == -1) 
         {
         // TODO: throw error or something
         }

      unsigned int dictionarySize = (tableSize + 2) * sizeof(unsigned int);
      start = (unsigned int *) mmap(nullptr, dictionarySize, PROT_READ | PROT_WRITE, MAP_PRIVATE, file, 0);
      start[0] = 1;
      start[1] = tableSize;
      dictionary = start + 2;

      nextAvailableLocation = (tableSize + 2) * sizeof(unsigned int);
      nextAvailableLocation += writeEODList(documents);
      }

   ~IndexChunkBuilder() 
      {
      munmap(start, (tableSize + 2) * sizeof(unsigned int));
      close(file);
      }

   void addWord(const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount);

private:

   void writeToBucket(const fb::String &word, unsigned int offset);

   void writePostingList(const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount);

   void writeEODList(const fb::Vector<DocIdInfo> &documents);

unsigned int * start;
unsigned int * dictionary;
uint32_t nextAvailableLocation;
uint32_t tableSize;
Hash hash;
int file;
};

template<typename Hash, int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
void IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::addWord
   (const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount)
   {
   fb::SizeT bucket = hash(word) % tableSize;
   while(dictionary[bucket] != 0) 
      {
      bucket = (bucket + 1) % tableSize;
      }

   dictionary[bucket] = nextAvailableLocation;
   
   nextAvailableLocation += writePostingList(word, offsets, docCount);
   }

template<typename Hash, int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
void IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::writePostingList
   (const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount) 
   {
   unsigned long maxListLength = (word.size() + 1) + (getSizeOfSkipTable(NUM_SKIP_TABLE_BITS)) + (4 * offsets.size());
   char * postingListLocation = (char *) mmap(nullptr, maxListLength, PROT_READ | PROT_WRITE, MAP_PRIVATE, file, nextAvailableLocation);

   PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK, DocIdInfo> builder(word, postingListLocation, docCount, offsets.size());
   for(const AbsoluteWordInfo &word : offsets) 
      {
      builder.addPost(word);
      }

   builder.endList();
   munmap(postingListLocation, maxListLength);
   return builder.getLength();
   }

template<typename Hash, int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
void IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::writeEODList(const fb::Vector<DocIdInfo> &documents) 
   {
   unsigned long maxListLength = 1 + (getSizeOfSkipTable(NUM_SKIP_TABLE_BITS)) + (12 + documents.size());
   char * postingListLocation = (char *) mmap(nullptr, maxListLength, PROT_READ | PROT_WRITE, MAP_PRIVATE, file, nextAvailableLocation);

   PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK, DocIdInfo> builder(fb::String(), postingListLocation, documents.size(), documents.size());
   for(const DocIdInfo &post : documents)
      {
      builder.addEODPost(post);
      }

   builder.endEODList();
   munmap(postingListLocation, maxListLength);
   return builder.getLength();
   }