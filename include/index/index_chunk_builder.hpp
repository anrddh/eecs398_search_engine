#pragma once

#include "posting_list_builder.hpp"
#include "index_data_structures.hpp"

int getHighestBit(int num)
   {
   int val = 0;
   while(num)
      {
      ++val;
      num >>= 1;
      }

   return val;
   }

constexpr long MAX_FILE_SIZE = 4000000000;

template<typename Hash, int NUM_SKIP_TABLE_BITS>
class IndexChunkBuilder {
public:
   IndexChunkBuilder(fb::String filename, uint32_t capacity, const fb::Vector<DocIdInfo> &documents, int num_tokens) 
      : tableSize(capacity), file(open(filename.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH)), MAX_TOKEN_BITS(getHighestBit(num_tokens)) 
      { 
      if(file == -1) 
         {
         // TODO: throw error or something
         }

      ftruncate(file, MAX_FILE_SIZE);

      start = (char *) mmap(nullptr, MAX_FILE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, file, 0);
      ((unsigned int*)start)[0] = 1;
      ((unsigned int*)start)[1] = num_tokens;
      ((unsigned int*)start)[2] = tableSize;
      dictionary = ((unsigned int*)start) + 3;
      std::memset(dictionary, 0, tableSize);

      nextAvailableLocation = (tableSize + 3) * sizeof(unsigned int);
      nextAvailableLocation += writeEODList(documents);
      }

   ~IndexChunkBuilder() 
      {
      munmap(start, MAX_FILE_SIZE);
      ftruncate(file, nextAvailableLocation);
      close(file);
      }

   void addWord(const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount);

private:

   void writeToBucket(const fb::String &word, unsigned int offset);

   int writePostingList(const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount);

   int writeEODList(const fb::Vector<DocIdInfo> &documents);

char * start;
unsigned int * dictionary;
uint32_t nextAvailableLocation;
uint32_t tableSize;
Hash hash;
int file;
int MAX_TOKEN_BITS;
};

template<typename Hash, int NUM_SKIP_TABLE_BITS>
void IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS>::addWord
   (const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount)
   {
   fb::SizeT bucket = hash(word) % tableSize;
   fb::SizeT originalBucket = bucket;
   if(dictionary[bucket] != 0)
      {
      ++bucket;
      while(dictionary[bucket] != 0) 
         {
         bucket = (bucket + 1) % tableSize;
         if(bucket == originalBucket)
            {
            perror("error empty bucket never found");
            }
         }
      }

   dictionary[bucket] = nextAvailableLocation;
   
   nextAvailableLocation += writePostingList(word, offsets, docCount);
   }

template<typename Hash, int NUM_SKIP_TABLE_BITS>
int IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS>::writePostingList
   (const fb::String &word, const fb::Vector<AbsoluteWordInfo> &offsets, unsigned int docCount) 
   {
   char * postingListLocation = start + nextAvailableLocation;

   PostingListBuilder<NUM_SKIP_TABLE_BITS> builder(word, postingListLocation, docCount, offsets.size(), MAX_TOKEN_BITS);
   for(const AbsoluteWordInfo &word : offsets) 
      {
      builder.addPost(word);
      }

   builder.endList();
   return builder.getLength();
   }

template<typename Hash, int NUM_SKIP_TABLE_BITS>
int IndexChunkBuilder<Hash, NUM_SKIP_TABLE_BITS>::writeEODList(const fb::Vector<DocIdInfo> &documents) 
   {
   char * postingListLocation = start + nextAvailableLocation;

   PostingListBuilder<NUM_SKIP_TABLE_BITS> builder(fb::String(), postingListLocation, documents.size(), documents.size(), MAX_TOKEN_BITS);
   for(const DocIdInfo &post : documents)
      {
      builder.addPost(post);
      }

   builder.endEODList();
   return builder.getLength();
   }