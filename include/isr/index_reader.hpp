#pragma once

#include "fb/memory.hpp"
#include "fb/functional.hpp"
#include "fb/string.hpp"

#include "index_reader_helpers.hpp"
#include "word_isr.hpp"
#include "word_impl_isr.hpp"
#include "empty_isr.hpp"
#include "document_isr.hpp"

#include "porter_stemmer.hpp"

class IndexReader
   {
public:
   IndexReader(const char * startOfIndex);
   ~IndexReader( ) { free_stemmer( porterStemmer ); }

   fb::UniquePtr<WordISR> OpenWordISR( fb::String word );
   fb::UniquePtr<WordISR> GetEmptyISR( );
   fb::UniquePtr<DocumentISR> OpenDocumentISR( );
   bool WordExists( fb::String word );

private:
   const char * start;
   const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
   stemmer * porterStemmer;
   };

IndexReader::IndexReader(const char * startOfIndex) 
: start(startOfIndex), 
   MAX_TOKEN_BITS( getHighestBit( *( ( ( unsigned int * ) start ) + 1 ) ) ), 
   DICTIONARY_SIZE( *( ( ( unsigned int * ) start ) + 2 ) ), 
   dictionary( ( ( unsigned int * ) start ) + 3 ),
   porterStemmer( create_stemmer( ) ) { }

fb::UniquePtr<WordISR> IndexReader::OpenWordISR( fb::String word )
   {
   int new_size = stem(porterStemmer, word.data( ), word.size( ) - 1) + 1;
   word.resize( new_size );
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;
   while(dictionary[bucket] && word.compare(start + dictionary[bucket]))
      {
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if(dictionary[bucket]) 
      {
      return fb::makeUnique<WordImplISR>(start + dictionary[bucket], OpenDocumentISR( ), MAX_TOKEN_BITS);
      }
   else
      {
      return fb::makeUnique<EmptyISR>( );
      }
   }

fb::UniquePtr<WordISR> IndexReader::GetEmptyISR( )
   {
   return fb::makeUnique<EmptyISR>( );
   }

fb::UniquePtr<DocumentISR> IndexReader::OpenDocumentISR( ) 
   {
   return fb::makeUnique<DocumentISR>((char * ) (dictionary + DICTIONARY_SIZE), MAX_TOKEN_BITS);
   }

bool IndexReader::WordExists( fb::String word )
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;
   while(dictionary[bucket] && strcmp(start + dictionary[bucket], word.data()))
      {
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      } 

   return dictionary[bucket];
   }