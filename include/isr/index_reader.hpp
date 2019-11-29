#pragma once

#include "fb/memory.hpp"
#include "fb/functional.hpp"
#include "fb/string.hpp"

#include "index_reader_helpers.hpp"
#include "word_isr.hpp"
#include "document_isr.hpp"

template<typename NUM_SKIP_TABLE_BITS>
class IndexReader
   {
public:
   IndexReader(char * startOfIndex);

   fb::UniquePtr<WordISR> OpenWordISR( fb::String word );
   fb::UniquePtr<DocumentISR> OpenDocumentISR( );

private:
   char * start;
   const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
    };

template<typename NUM_SKIP_TABLE_BITS>
IndexReader<NUM_SKIP_TABLE_BITS>::IndexReader(char * startOfIndex) 
: start(startOfIndex), 
   MAX_TOKEN_BITS( getHighestBit( *( ( ( unsigned int * ) start ) + 1 ) ) ), 
   DICTIONARY_SIZE( *( ( ( unsigned int * ) start ) + 2 ) ), 
   dictionary( ( ( unsigned int * ) start ) + 3 ) { }

template<typename NUM_SKIP_TABLE_BITS>
fb::UniquePtr<WordISR> IndexReader<NUM_SKIP_TABLE_BITS>::OpenWordISR( fb::String word )
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;
   while(dictionary[bucket] && strcmp(start + dictionary[bucket], word.data()))
      {
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if(dictionary[bucket]) 
      {
      fb::UniquePtr<WordISR> wordISR = fb::makeUnique(start + dictionary[bucket], OpenDocumentISR( ), NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS);
      return wordISR;
      }
   else
      {
      return fb::UniquePtr<WordISR>();
      }
   }

template<typename NUM_SKIP_TABLE_BITS>
fb::UniquePtr<DocumentISR> IndexReader<NUM_SKIP_TABLE_BITS>::OpenDocumentISR( ) 
   {
   fb::UniquePtr<DocumentISR> docISR = fb::makeUnique(dictionary + DICTIONARY_SIZE, NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS);
   return docISR;
   }