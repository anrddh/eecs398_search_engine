#pragma once

#include <string.h>

#include <fb/memory.hpp>
#include <fb/functional.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>

#include <isr/word_impl_isr.hpp>
#include <isr/index_reader_helpers.hpp>
#include <isr/word_isr.hpp>
#include <isr/empty_isr.hpp>
#include <isr/document_isr.hpp>

#include "porter_stemmer.hpp"

class DocumentISR;
class WordImplISR;

class IndexReader {
public:
   IndexReader(const char * startOfIndex, uint32_t index_);

   IndexReader( );

   IndexReader( IndexReader &other );

   IndexReader( IndexReader &&other );

   IndexReader& operator=( IndexReader &other);

   IndexReader& operator=( IndexReader &&other);

   ~IndexReader( ) { free_stemmer( porterStemmer ); }

   fb::SizeT getIndex( ) { return index; }

   fb::UniquePtr<WordISR> OpenWordISR( fb::String word );
   fb::UniquePtr<WordISR> OpenPlainWordISR( fb::String &word );
   fb::UniquePtr<WordISR> GetEmptyISR( );
   fb::UniquePtr<DocumentISR> OpenDocumentISR( );
   bool WordExists( fb::String &word );
   bool deleteWord( fb::String &word );
   unsigned int GetSizeOfTable( );
   fb::Pair<int, fb::String> GetNextWord( int index );
   fb::SizeT GetNumTokens( );

private:
   int getBucket( fb::String &word );

   const char * start;
   unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
   fb::SizeT index;
   stemmer * porterStemmer;
   };
