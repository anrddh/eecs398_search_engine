#pragma once 

#include <stdio.h> // perror

#include "isr.hpp"
#include "document_isr.hpp"
#include "index_reader.hpp"
#include "index_reader_helpers.hpp"

template<int NUM_SKIP_TABLE_BITS>
class IndexReader;

class WordInfo : public IndexInfo 
   {
public:
   WordInfo(Location postLoc) : loc(postLoc) { }
   ~WordInfo( ) { }
   virtual Location GetStartLocation( )
      {
      return loc;
      }

   virtual Location GetEndLocation( )
      {
      return loc;
      }

private:
   Location loc;
   };

class WordISR : public ISR
   {
public:
   WordISR(const char * location, fb::UniquePtr<DocumentISR> documentISR, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS);
   WordISR(WordISR && other) = default;
   ~WordISR( ) { }
   unsigned int GetDocumentCount( );
   unsigned int GetNumberOfOccurrences( );
   virtual uint32_t GetDocumentId( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );

private:
   fb::UniquePtr<DocumentISR> docISR;
   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;
   };

WordISR::WordISR(const char * location, fb::UniquePtr<DocumentISR> documentISR, int NUM_SKIP_TABLE_BITS_, int MAX_TOKEN_BITS_) 
   : docISR(std::move(documentISR)),
      NUM_SKIP_TABLE_BITS(NUM_SKIP_TABLE_BITS_), 
      MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
      absolutePosition(0),
      skipTable((unsigned int *)findSkipTable(location)), 
      rankingData(skipTable - 2),
      currentLocation( ( ( const char * ) skipTable ) + ( 1 << NUM_SKIP_TABLE_BITS ) * 2 * sizeof( unsigned int ) ),
      start(location),
      isAtEnd(fb::is_word_sentinel(currentLocation))
   {
   if(!isAtEnd) 
      {
      currentLocation = fb::read_word_post(currentLocation, absolutePosition);
      }
   }

unsigned int WordISR::GetDocumentCount( )
   {
   return rankingData[0];
   }

unsigned int WordISR::GetNumberOfOccurrences( )
   {
   return rankingData[1];
   }

uint32_t WordISR::GetDocumentId( )
   {
   return docISR->GetDocumentId( );
   }

fb::UniquePtr<IndexInfo> WordISR::GetCurrentInfo( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   return fb::makeUnique<WordInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordISR::Next( ) 
   {
   if(isAtEnd || fb::is_word_sentinel( currentLocation )  )
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }
   
   uint32_t delta;
   currentLocation = fb::read_word_post(currentLocation, delta);

   absolutePosition += delta;
   return fb::makeUnique<WordInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordISR::NextDocument( )
   {
   if( isAtEnd || fb::is_word_sentinel( currentLocation ) ) 
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   fb::UniquePtr<IndexInfo> docInfo = docISR->Seek( absolutePosition );
   
   if(!docInfo)
      {
      perror("ERROR, word with no EOD after it");
      }

   return Seek( docInfo->GetStartLocation( ) );
   }

fb::UniquePtr<IndexInfo> WordISR::Seek( Location target )
   {
   isAtEnd = false;
   int index = target >> (MAX_TOKEN_BITS - NUM_SKIP_TABLE_BITS);
   currentLocation = start + skipTable[2 * index];
   absolutePosition = skipTable[2 * index + 1];
   if(!absolutePosition)
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>();
      }
   
   uint32_t trash;
   currentLocation = fb::read_word_post(currentLocation, trash); // move past first element

   while( absolutePosition < target && Next( ) )
      ;

   return GetCurrentInfo( );
   }

bool WordISR::AtEnd( ) 
   {
   return isAtEnd;
   }
