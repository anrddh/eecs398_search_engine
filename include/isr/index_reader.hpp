#pragma once

#include "fb/memory.hpp"
#include "fb/functional.hpp"
#include "fb/string.hpp"
#include "fb/stddef.hpp"

#include "index_reader_helpers.hpp"
#include "word_isr.hpp"
#include "empty_isr.hpp"

#include "porter_stemmer.hpp"

class DocumentISR;

class IndexReader
   {
public:
   IndexReader(const char * startOfIndex, fb::SizeT index);
   ~IndexReader( ) { free_stemmer( porterStemmer ); }

   fb::UniquePtr<WordISR> OpenWordISR( fb::String word );
   fb::UniquePtr<DocumentISR> OpenDocumentISR( );
   bool WordExists( fb::String word );
   fb::SizeT getIndex( ) { return index; }

private:
   const char * start;
   const unsigned int MAX_TOKEN_BITS, DICTIONARY_SIZE;
   unsigned int * dictionary;
   fb::SizeT index;
   stemmer * porterStemmer;
   };
