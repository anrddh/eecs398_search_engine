#include <isr/index_reader.hpp>
#include <isr/word_impl_isr.hpp>
#include <isr/document_isr.hpp>
#include <stdint.h>



IndexReader::IndexReader( )
: start( nullptr ),
MAX_TOKEN_BITS( 0 ),
DICTIONARY_SIZE( 0 ),
dictionary( nullptr ),
index( 0 ),
porterStemmer( create_stemmer( ) ) { }

IndexReader::IndexReader( IndexReader &other )
: start(other.start),
MAX_TOKEN_BITS( other.MAX_TOKEN_BITS),
DICTIONARY_SIZE( other.DICTIONARY_SIZE ),
dictionary( other.dictionary ),
index( other.index ),
porterStemmer( create_stemmer( ) ) { }

IndexReader::IndexReader( IndexReader &&other )
: start(other.start),
MAX_TOKEN_BITS( other.MAX_TOKEN_BITS),
DICTIONARY_SIZE( other.DICTIONARY_SIZE ),
dictionary( other.dictionary ),
index( other.index ),
porterStemmer( create_stemmer( ) ) { }

IndexReader::IndexReader(const char * startOfIndex, uint32_t index_)
    : start(startOfIndex),
      MAX_TOKEN_BITS( getHighestBit( *( ( ( unsigned int * ) start ) + 1 ) ) ),
      DICTIONARY_SIZE( *( ( ( unsigned int * ) start ) + 2 ) ),
      dictionary( ( ( unsigned int * ) start ) + 3 ),
      index(index_),
      porterStemmer( create_stemmer( ) ) { }

IndexReader& IndexReader::operator=( IndexReader &other)
    {
    if(this == &other)
        {
        return *this;
        }
    else
        {
        start = other.start;
        MAX_TOKEN_BITS = other.MAX_TOKEN_BITS;
        DICTIONARY_SIZE = other.DICTIONARY_SIZE;
        dictionary = other.dictionary;
        index = other.index;
        return *this;
        }
    }

IndexReader& IndexReader::operator=( IndexReader &&other)
    {
    if(this == &other)
        {
        return *this;
        }
    else  
        {
        start = other.start;
        MAX_TOKEN_BITS = other.MAX_TOKEN_BITS;
        DICTIONARY_SIZE = other.DICTIONARY_SIZE;
        dictionary = other.dictionary;
        index = other.index;
        return *this;
        }
    }

fb::UniquePtr<WordISR> IndexReader::OpenPlainWordISR(fb::String &word)
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;

    while (dictionary[bucket] && (dictionary[bucket] == UINT32_MAX || word.compare(start + dictionary[bucket])))
      {
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if (dictionary[bucket] && dictionary[bucket] != UINT32_MAX)
      {
      return fb::makeUnique<WordImplISR>(start + dictionary[bucket],
                                           OpenDocumentISR(), MAX_TOKEN_BITS);
      }
   else
      {
      return fb::makeUnique<EmptyISR>();
      }

   }

fb::UniquePtr<WordISR> IndexReader::OpenWordISR(fb::String &word) {
   int new_size = stem(porterStemmer, word.data(), word.size() - 1) + 1;
   word.resize(new_size);
   return OpenPlainWordISR( word );
}

fb::UniquePtr<WordISR> IndexReader::GetEmptyISR() {
    return fb::makeUnique<EmptyISR>();
}

fb::UniquePtr<DocumentISR> IndexReader::OpenDocumentISR() {
    return fb::makeUnique<DocumentISR>((char * ) (dictionary + DICTIONARY_SIZE),
                                       MAX_TOKEN_BITS);
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
      dictionary[bucket] = UINT32_MAX;
      return true;
      }
   }

// returns the bucket that contains word or -1
int IndexReader::getBucket( fb::String &word )
   {
   fb::Hash<fb::String> hash;
   uint64_t bucket = hash(word) % DICTIONARY_SIZE;
   uint64_t beg = bucket;
   bool pastStart = false;
   
   while( dictionary[bucket] && (dictionary[bucket] == UINT32_MAX || strcmp(start + dictionary[bucket], word.data())) && ( bucket != beg || !pastStart ) )
      {
      pastStart = true;
      bucket = (bucket + 1) % DICTIONARY_SIZE;
      }

   if(dictionary[bucket] && dictionary[bucket] != UINT32_MAX)
      {
      return bucket;
      }
   else
      {
      return -1;
      }
   }

unsigned int IndexReader::GetSizeOfTable( )
   {
   return DICTIONARY_SIZE;
   }

fb::Pair<int, fb::String> IndexReader::GetNextWord( int beg )
   {
   unsigned int bucket = beg;

   while( ( !dictionary[bucket] || dictionary[bucket] == UINT32_MAX ) &&  bucket < DICTIONARY_SIZE )
      {
      ++bucket;
      }

   if(bucket >= DICTIONARY_SIZE || !dictionary[bucket] || dictionary[bucket] == UINT32_MAX)
      {
      return fb::make_pair(-1, fb::String(""));
      }
   else
      {
      return fb::make_pair( int((bucket) % DICTIONARY_SIZE), fb::String(start + dictionary[bucket]));
      }
   }

fb::SizeT IndexReader::GetNumTokens( )
   {
   unsigned int * stats = (unsigned int *) start;
   return stats[1];
   }