#pragma once

#include "index_reader.hpp"
#include "index_reader_helpers.hpp"
#include "isr.hpp"

class DocumentInfo : public IndexInfo
   {
public:
   DocumentInfo(Location postLoc) : loc(postLoc) { }
   ~DocumentInfo( ) { }
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

class DocumentISR : public ISR 
   {
public:
   DocumentISR(const char * location, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS); 
   ~DocumentISR( ) { }
   unsigned GetDocumentLength( );
   unsigned int GetDocumentCount( );
   unsigned int GetDocumentId( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );


private:
   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition, docId;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;

   };

DocumentISR::DocumentISR(const char * location, int NUM_SKIP_TABLE_BITS_, int MAX_TOKEN_BITS_) 
: NUM_SKIP_TABLE_BITS(NUM_SKIP_TABLE_BITS_), 
   MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
   absolutePosition(0),
   docId(0),
   skipTable((unsigned int *)findSkipTable(location)), 
   rankingData(skipTable - 2),
   currentLocation( ( const char * ) ( skipTable + ( 1 << NUM_SKIP_TABLE_BITS ) * 2 ) ),
   start(location),
   isAtEnd(fb::is_document_sentinel(currentLocation)) 
   {
   if(!isAtEnd)
      {
      currentLocation = fb::read_document_post(currentLocation, absolutePosition, docId);
      }
   }

unsigned int DocumentISR::GetDocumentLength( )
   {
   uint32_t delta;
   fb::read_document_post(currentLocation, delta, docId);
   return delta - 1;
   }

unsigned int DocumentISR::GetDocumentCount( )
   {
   return rankingData[0];
   }

unsigned int DocumentISR::GetDocumentId( )
   {
   return docId;
   }

fb::UniquePtr<IndexInfo> DocumentISR::GetCurrentInfo( ) 
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   
   return fb::makeUnique<DocumentInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> DocumentISR::Next( )
   {
   if(isAtEnd || fb::is_document_sentinel( currentLocation ) ) 
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   uint32_t delta;
   currentLocation = fb::read_document_post(currentLocation, delta, docId);

   absolutePosition += delta;
   return fb::makeUnique<DocumentInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> DocumentISR::NextDocument( )
   {
   return Next( );
   }

fb::UniquePtr<IndexInfo> DocumentISR::Seek( Location target )
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
   currentLocation = fb::read_document_post(currentLocation, trash, docId);

   while( absolutePosition < target && Next( ) )
      ;

   return GetCurrentInfo( );
   }

bool DocumentISR::AtEnd( )
   {
   return isAtEnd;
   }