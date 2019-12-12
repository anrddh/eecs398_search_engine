#pragma once

#include "fb/memory.hpp"
#include "fb/functional.hpp"
#include "fb/string.hpp"
#include "fb/stddef.hpp"

#include "index_reader_helpers.hpp"
#include "word_isr.hpp"
#include "empty_isr.hpp"

#include "porter_stemmer.hpp"

class WordImplISR;
class DocumentISR;

class IndexReader {
public:
<<<<<<< HEAD
   IndexReader(const char * startOfIndex, fb::SizeT index);
   IndexReader( IndexReader &other);
   ~IndexReader( ) { free_stemmer( porterStemmer ); }

   fb::UniquePtr<WordISR> OpenWordISR( fb::String &word );
   fb::UniquePtr<DocumentISR> OpenDocumentISR( );
   bool WordExists( fb::String &word );
   fb::SizeT getIndex( ) { return index; }
   bool deleteWord( fb::String &word );

private:
   int getBucket( fb::String &word );

   const char * start;
   const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
   fb::SizeT index;
   stemmer * porterStemmer;
   };

IndexReader::IndexReader(const char * startOfIndex, fb::SizeT index)
: start(startOfIndex),
   MAX_TOKEN_BITS( getHighestBit( *( ( ( unsigned int * ) start ) + 1 ) ) ),
   DICTIONARY_SIZE( *( ( ( unsigned int * ) start ) + 2 ) ),
   dictionary( ( ( unsigned int * ) start ) + 3 ),
   index(index);
   porterStemmer( create_stemmer( ) ) { }

IndexReader::IndexReader( IndexReader &other);
   : start(other.start),
   MAX_TOKEN_BITS( other.MAX_TOKEN_BITS),
   DICTIONARY_SIZE( other.DICTIONARY_SIZE ),
   dictionary( other.dictionary ),
   index( other.index );
   porterStemmer( create_stemmer( ) ) { }

fb::UniquePtr<WordISR> IndexReader::OpenWordISR( fb::String &word )
   {
   int new_size = stem(porterStemmer, word.data( ), word.size( ) - 1) + 1;
   word.resize( new_size );
   int bucket = getBucket( word );
   

   if( bucket != -1 )
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

bool IndexReader::WordExists( fb::String &word )
   {
   return getBucket( word ) != -1; 
   }


bool IndexReader::deleteWord( fb::String &word )
   {
   int bucket = getBucket( word );
   if(bucket == -1)
      {
      return false;
      }
   else
      {
      dictionary[bucket] = 0;
      }
   }

// returns the bucket that contains word or -1
int IndexReader::getBucket( fb::String &word )
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;
   uint64_t start = bucket;
   bool pastStart = false;
   
   while( dictionary[bucket] && strcmp(start + dictionary[bucket], word.data()) && ( bucket != start || !pastStart ) )
      {
      pastStart = true;
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if(dictionary[bucket])
      {
      return bucket;
      }
   else
      {
      return -1;
      }
   }
=======
    IndexReader(const char * startOfIndex, uint32_t index);
    ~IndexReader( ) { free_stemmer( porterStemmer ); }

    fb::UniquePtr<WordISR> OpenWordISR( fb::String word );
    fb::UniquePtr<DocumentISR> OpenDocumentISR( );
    bool WordExists( fb::String word );
    fb::UniquePtr<WordISR> GetEmptyISR( );
    uint32_t getIndex( ) { return index; }

private:
    const char * start;
    const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
    unsigned int * dictionary;
    uint32_t index;
    stemmer * porterStemmer;
};
>>>>>>> db069194cd98dd5585f52599a831bb1a83949fac
