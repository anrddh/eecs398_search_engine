#pragma once

#include <type_traits>

#include "fb/indexEntry.hpp"
#include "fb/string.hpp"

#include "index/index_helpers.hpp"
#include "index/index_data_structures.hpp"

struct EODPost {
   size_t position;
   uint64_t url_uid;
};

template<int NUM_SKIP_TABLE_BITS>
class PostingListBuilder 
   {
public:

   PostingListBuilder(const fb::String &word, char * location, int numDocs, int numOccurences, int MAX_TOKEN_BITS_) 
      : beginning(location), lastLocation(0), nextSkipTableEntry(0), MAX_TOKEN_BITS(MAX_TOKEN_BITS_)
      {
      strcpy(beginning, word.data());
      
      unsigned int * rankingData = (unsigned int *) (beginning + (word.size() + 1));

      rankingData[0] = numDocs;
      rankingData[1] = numOccurences;

      skipTableStart = (unsigned int *) (beginning + (word.size() + 1) + getSizeOfRankingData()); // past word and past the num of documents and num of occurences of word
      std::memset(skipTableStart, 0, getSizeOfSkipTable(NUM_SKIP_TABLE_BITS)); // 0 out skip table

      currentPostPosition = beginning + (word.size() + 1) + getSizeOfRankingData() + getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);

      }

   template<typename PostType>
   void addPost(PostType post)
      {
      while( ( post.position >> ( MAX_TOKEN_BITS - NUM_SKIP_TABLE_BITS ) ) >= nextSkipTableEntry ) 
         {
         skipTableStart[2 * nextSkipTableEntry] = currentPostPosition - beginning;
         skipTableStart[2 * nextSkipTableEntry + 1] = post.position;
         +;
         }

      writePost(post);
      }

   unsigned int getLength() 
      {
      return currentPostPosition - beginning;
      }

   void endList() 
      {
      currentPostPosition = fb::add_word_sentinel(currentPostPosition);
      }

   void endEODList() 
      {
      currentPostPosition = fb::add_document_sentinel(currentPostPosition);
      }

   template<typename PostType>
   fb::EnableIfT<fb::IsSameV<PostType, AbsoluteWordInfo>, void>
   writePost(PostType post) 
      {
      currentPostPosition = fb::add_word_post(currentPostPosition, post.position - lastLocation);
      lastLocation = post.position;
      }

   template<typename PostType> 
   fb::EnableIfT<fb::IsSameV<PostType, DocIdInfo>, void>
   writePost(PostType post) 
      {
      currentPostPosition = fb::add_document_post(currentPostPosition, post.position - lastLocation, post.docId);
      lastLocation = post.position;
      }

private:

   char * beginning;
   char * currentPostPosition;
   unsigned int * skipTableStart;
   unsigned int lastLocation;
   unsigned int nextSkipTableEntry;
   int MAX_TOKEN_BITS;
   };
