#pragma once

#include <type_traits>

#include "fb/indexEntry.hpp"

#include "index_helpers.hpp"
#include "index_data_structures.hpp"

struct EODPost {
   size_t position;
   uint64_t url_uid;
};

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK, typename PostType>
class PostingListBuilder 
   {
public:

   PostingListBuilder(const fb::String &word, char * location, int numDocs, int numOccurences) : beginning(location), lastLocation(0) 
      {
      strcpy(beginning, word.data());
      
      unsigned int * rankingData = (unsigned int *) (beginning + (word.size() + 1));

      rankingData[0] = numDocs;
      rankingData[1] = numOccurences;

      skipTableStart = (unsigned int *) (beginning + (word.size() + 1) + getSizeOfRankingData()); // past word and past the num of documents and num of occurences of word
      memset(skipTableStart, 0, getSizeOfSkipTable); // 0 out skip table

      currentPostPosition = beginning + (word.size() + 1) + getSizeOfRankingData() + getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);

      }

   void addPost(PostType post)
      {
      if((post.position >> (MAX_BITS_PER_CHUNK - NUM_SKIP_TABLE_BITS)) > nextSkipTableEntry) 
         {
         skipTableStart[2 * nextSkipTableEntry] = currentPostPosition - beginning;
         skipTableStart[2 * nextSkipTableEntry + 1] = post.position;
         ++nextSkipTableEntry;
         }

      currentPostPosition = writePost(post);
      lastLocation = post.position;
      }

   unsigned int getLength() 
      {
      return currentPostPosition - beginning;
      }

   void endList() 
      {
      AbsoluteWordInfo post;
      post.position = 0;
      post.type_flags = 0;
      writePost(post);
      }

   void writePost(AbsoluteWordInfo post) 
      {
      currentPostPosition = add_word_post(currentPostPosition, post.position - lastLocation, post.type_flags);
      }

   void endEODList() 
      {
      DocIdInfo post;
      post.position = 0;
      post.docId = 0;
      writePost(post);
      }

   void writeEODPost(DocIdInfo post) 
      {
      currentPostPosition = add_document_post(currentPostPosition, post.position - lastLocation, post.docId);
      }

private:

   char * beginning;
   char * currentPostPosition;
   unsigned int * skipTableStart;
   unsigned int lastLocation;
   unsigned int nextSkipTableEntry = 0;
   };

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
class PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK, AbsoluteWordInfo>
   {
public:
   
   };

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
class PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK, DocIdInfo>
   {
public:
   
   };