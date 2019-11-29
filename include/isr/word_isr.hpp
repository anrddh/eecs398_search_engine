#pragma once 

#include <stdio.h> // perror

#include "isr.hpp"
#include "document_isr.hpp"
#include "index_reader.hpp"
#include "index_reader_helpers.hpp"

class WordInfo : IndexInfo 
   {
public:
   PostInfo(Location postLoc) : loc(postLoc) { }
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
   unsigned int GetDocumentCount( );
   unsigned int GetNumberOfOccurrences( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );

protected:
   friend class IndexReader;

   WordISR(char * location, fb::UniquePtr<ISRDocument> documentISR, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS);

   fb::UniquePtr<DocumentISR> docISR;
   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;
   };

WordISR::ISRWord(char * location, fb::UniquePtr<DocumentISR> documentISR, int NUM_SKIP_TABLE_BITS_, int MAX_TOKEN_BITS_) 
   : docISR(std::move(documentISR)),
      NUM_SKIP_TABLE_BITS(NUM_SKIP_TABLE_BITS_), 
      MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
      absolutePosition(0),
      skipTable((unsigned int *)findSkipTable(location)), 
      rankingData(skipTable - 2),
      currentLocation(skipTable + (2 << NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int)),
      start(location),
      isAtEnd(false)
   { 
   Next( );
   }

unsigned int WordISR::GetDocumentCount( )
   {
   return rankingData[0];
   }

unsigned int WordISR::GetNumberOfOccurrences( )
   {
   return rankingData[1];
   }

fb::UniquePtr<IndexInfo> WordISR::GetCurrentInfo( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   return fb::makeUnique(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordISR::Next( ) 
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   
   uint32_t delta;
   currentLocation = fb::read_word_post(currentLocation, delta);

   if( fb::is_word_sentinel( currentLocation ) ) 
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   absolutePosition += delta;
   return fb::makeUnique(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordISR::NextDocument( )
   {
   if( isAtEnd ) 
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   fb::UniquePtr<PostInfo> currentPostInfo = GetCurrentInfo( );

   fb::UniquePtr<IndexInfo> docInfo = docISR->Seek( currentPostInfo->GetStartLocation( ) );
   
   if(!docInfo)
      {
      perror("ERROR, word with no EOD after it");
      }

   return Seek( info->GetStartLocation( ) );
   }

fb::UniquePtr<IndexInfo> WordISR::Seek( Location target )
   {
   isAtEnd = false;
   int index = target >> (MAX_TOKEN_BITS - NUM_SKIP_TABLE_BITS);
   currentLocation = skipTable[2 * index];
   absolutePosition = skipTable[2 * index + 1];
   if(!absolutePosition)
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>();
      }

   return GetCurrentInfo( );
   }

bool WordISR::AtEnd( ) 
   {
   return isAtEnd;
   }
