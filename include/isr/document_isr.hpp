#pragma once

#include "index_reader.hpp"
#include "index_reader_helpers.hpp"
#include "isr.hpp"

class DocumentInfo : IndexInfo
   {
public:
   DocumentInfo(Location postLoc, uint32_t documentID) : loc(postLoc), docID(documentID) { }
   virtual Location GetStartLocation( )
      {
      return loc;
      }

   virtual Location GetEndLocation( )
      {
      return loc;
      }

   uint32_t GetDocId( )
      {
      return docID;
      }

private:
   Location loc;
   uint32_t docID;
   };

class DocumentISR : ISR 
   {
public:
   unsigned GetDocumentLength( );
   unsigned int GetDocumentCount( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );
   ~DocumentISR( );


private:
   friend class IndexReader;

   DocumentISR(char * location, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS); 

   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition, docID;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;

   };

DocumentISR::DocumentISR(char * location, int NUM_SKIP_TABLE_BITS_, int MAX_TOKEN_BITS_) 
: NUM_SKIP_TABLE_BITS(NUM_SKIP_TABLE_BITS_), 
   MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
   absolutePosition(0),
   skipTable((unsigned int *)findSkipTable(location)), 
   rankingData(skipTable - 2),
   currentLocation( ( const char * ) ( skipTable + ( 2 << NUM_SKIP_TABLE_BITS ) * 2 * sizeof( unsigned int ) ) ),
   start(location),
   isAtEnd(false) { }

unsigned int DocumentISR::GetDocumentLength( )
   {
   uint32_t delta;
   fb::read_document_post(currentLocation, delta, docID);
   return delta - 1;
   }

unsigned int DocumentISR::GetDocumentCount( )
   {
   return rankingData[0];
   }

fb::UniquePtr<IndexInfo> DocumentISR::GetCurrentInfo( ) 
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   
   return fb::makeUnique<DocumentInfo>(absolutePosition, docID);
   }

fb::UniquePtr<IndexInfo> DocumentISR::Next( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   
   uint32_t delta;
   currentLocation = fb::read_document_post(currentLocation, delta, docID);

   if( fb::is_document_sentinel( currentLocation ) ) 
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   absolutePosition += delta;
   return fb::makeUnique<DocumentInfo>(absolutePosition, docID);
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

   return GetCurrentInfo( );
   }

bool DocumentISR::AtEnd( )
   {
   return isAtEnd;
   }