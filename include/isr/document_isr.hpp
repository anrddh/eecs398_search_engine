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
   DocumentISR(const char * location, int MAX_TOKEN_BITS);
   DocumentISR(DocumentISR &&other) = default;
   ~DocumentISR( ) { }
   unsigned GetDocumentLength( );
   unsigned int GetDocumentCount( );
   virtual unsigned int GetDocumentId( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );


private:
   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition, docId, docLength;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;

   };
