#pragma once

#include "fb/memory.hpp"
#include "fb/functional.hpp"

#include "index_reader_helpers.hpp"

class WordISR;
class DocumentISR;

template<typename NUM_SKIP_TABLE_BITS>
class IndexReader
   {
public:
   IndexReader(char * startOfIndex);

   fb::UniquePtr<WordISR> OpenISRWord( char *word );
   fb::UniquePtr<DocumentISR> OpenISREndDoc( );

private:
   char * start;
   const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
    };

IndexReader::IndexReader(char * startOfIndex) 
: start(startOfIndex), 
   MAX_TOKEN_BITS(getHighestBit(((unsigned int *) start) + 1 )), 
   DICTIONARY_SIZE( ( ( unsigned int * ) start ) + 2 ), 
   dictionary(( ( unsigned int * ) start ) + 3 ) { }

fb::UniquePtr<WordISR> IndexReader::OpenISRWord( char *word )
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(fb::String(word)) % DICTIONARY_SIZE;
   while(dictionary[bucket] && strcmp(start + dictionary[bucket], word))
      {
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if(dictionary[bucket]) 
      {
      fb::UniquePtr<WordISR> wordISR = fb::makeUnique(start + dictionary[bucket], OpenISRDocument, NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS);
      return wordISR;
      }
   else
      {
      return fb::UniquePtr<WordISR>();
      }
   }

fb::UniquePtr<DocumentISR> IndexReader::OpenISREndDoc( ) 
   {
   fb::UniquePtr<DocumentISR> docISR = fb::makeUnique(dictionary + DICTIONARY_SIZE, NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS);
   return docISR;
   }