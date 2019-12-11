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
